classdef ImgStream < handle
%% private properties
properties (Access=private)
    listenTimer;
end
%%    
properties(SetAccess=protected)
    LocalPort=10000;
    remoteHost='127.0.0.1';
    remotePort=12345;
    
    IsConnected=0;
    
    img;
end
%% constants
properties(Constant, Access=private)
    udpDelay=1e-10;
    PACK_SIZE=4096;
end
%% private properties
properties(Access=private)
    udpHandle;
    
    t0=0;
    t1=0;
    firstTimeimgHandle=1;
    
    nPackets=0;
    packetCounter=0;
    timeout=5; % seconds
    
    imageHandle;
end
%% puclick properties
properties
    showImage=0;
end
%% public methods
methods
    
    % constructor
    function obj = ImgStream()
        
        obj.listenTimer=timer;
        obj.listenTimer.Period=0.001;
        obj.listenTimer.ExecutionMode='fixedSpacing';
        obj.listenTimer.TimerFcn={@(tmrobj,event)ListenCalback(obj,tmrobj,event)};
    end
    function set_LocalPort(obj,value)
        if ~isnumeric(value)
            error('Value must be numeric.');
        end
        if value <1
            error('Value must be positive.');
        end
        
        obj.LocalPort=value; 
    end
    
    function set_remoteHost(obj,value)
        if ~ischar(value)
            error('Value must be string.');
        end
        obj.remoteHost=value;
    end
    
    function Connect(obj)
        if obj.IsConnected
            disp('Already connected')
        else
            try
            obj.udpHandle=pnet('udpsocket',obj.LocalPort,'noblock');
            pnet(obj.udpHandle,'setreadtimeout',obj.udpDelay);
            pnet(obj.udpHandle,'setwritetimeout',obj.udpDelay);
            catch
                error('Conneciton error. Make sure pnet UDP class is included and/or local port is available.');
            end
            
            obj.t0=tic;
            start(obj.listenTimer);
            obj.IsConnected=1;
        end
    end
    
    function Disconnect(obj)
        stop(obj.listenTimer);
        % close UDP
        try
            stat=pnet(obj.udpHandle,'status');
            if stat>0
                pnet(obj.udpHandle,'close')
                obj.udpHandle=[];
                disp(['local UDP Port: ', num2str(obj.LocalPort), ' is closed!'])

            end
        catch
               disp('Could not close port. Might be already closed.');
        end
        pnet('closeall')
        
        obj.reset();
        obj.IsConnected=0;
    end
    
    function reset(obj)
        obj.firstTimeimgHandle=1;
        obj.nPackets=0;
        obj.t0=0; obj.t1=0;
        obj.packetCounter=0;
    end
    
    function delete(obj)
        stop(obj.listenTimer)
        
        if isvalid(obj.listenTimer)
            stop(obj.listenTimer);
            delete(obj.listenTimer);
        end
        % close UDP
        try
            stat=pnet(obj.udpHandle,'status');
            if stat>0
                pnet(obj.udpHandle,'close')
                obj.udpHandle=[];
                disp(['local UDP Port: ', num2str(obj.LocalPort), ' is closed!'])

            end
        catch
                %warning('UDP socket is already closed.')
        end
        pnet('closeall')
        
        disp('Object deleted.');
        
    end
end
%% private method
methods (Access=private)
    
    function ListenCalback(obj,~,~)
                len=pnet(obj.udpHandle,'readpacket','noblock');
                data=pnet(obj.udpHandle,'read',5000,'uint8');
                % state 1: receive number of image packets
                if (len ==12)
                    obj.t0=tic;

                    ID=typecast(data,'int32');
                    %disp('got number of packets')
                    obj.nPackets=ID(3);

                    % acknowledge that we got the total number of image packets
                    pnet(obj.udpHandle,'write',typecast(int32(obj.nPackets),'uint8'));              % Write to write buffer
                    pnet(obj.udpHandle,'writepacket',obj.remoteHost,obj.remotePort,'noblock');   % Send buffer as UDP packet

                    obj.t1=tic;
                    obj.packetCounter=0;
                    longbuff=uint8(zeros(1,obj.PACK_SIZE*obj.nPackets));
                    while (obj.packetCounter < obj.nPackets)
                        % we took long time
                        if (toc(obj.t1)>obj.timeout)
                            disp('Timeout. Closing...')
                            obj.Disconnect();
                            break;
                        end
                        % read one image packet

                        len=pnet(obj.udpHandle,'readpacket','noblock');
                        data=pnet(obj.udpHandle,'read',5000,'uint8');

                        if len == obj.PACK_SIZE
                            longbuff((obj.packetCounter*obj.PACK_SIZE+1):(obj.packetCounter*obj.PACK_SIZE+obj.PACK_SIZE))=data;
                            obj.packetCounter=obj.packetCounter+1;

                            % acknowledge that we got one image packet
                            pnet(obj.udpHandle,'write',typecast(int32(999),'uint8'));% Write to write buffer
                            pnet(obj.udpHandle,'writepacket',obj.remoteHost,obj.remotePort);   % Send buffer as UDP packet
                        end

                    end % end of receiving image packets. Next, decode them.

                    if (obj.nPackets==obj.packetCounter)
                        obj.img=decodeUDPimg(longbuff,obj.packetCounter*obj.PACK_SIZE);
                        if obj.showImage
                            if obj.firstTimeimgHandle<2
                                image(obj.img);
                                obj.imageHandle = get(gca,'Children');
                                %obj.imageHandle.CDataMapping='scaled';
                                %drawnow;
                                obj.firstTimeimgHandle=obj.firstTimeimgHandle+1;
                            else
                                set(obj.imageHandle ,'CData',obj.img);
                            end
                        else
                            obj.firstTimeimgHandle=1;
                        end
                    end
                else
                    if toc(obj.t0)>obj.timeout
                        % we took long time to receive packets
                        disp('Timeout. Closing...')
                        obj.Disconnect();
                    end
                end% end of receiving and decoding one image
            end
end
end