lport=10000; % local port

% remote port and IP
remotehost='127.0.0.1';
remoteport=12345;

udpo=pnet('udpsocket',lport);
sec=0.1;
pnet(udpo,'setreadtimeout',sec)
pnet(udpo,'setwritetimeout',sec)
%%
nPackets=0;
wrong_packets=false;
PACK_SIZE=4096;

t0=tic;
try
    while(1)
        len=pnet(udpo,'readpacket','noblock');
        data=pnet(udpo,'read',5000,'uint8');
        % state 1: receive number of image packets
        if (len ==12)
            t0=tic;

            ID=typecast(data,'int32');
            %disp('got number of packets')
            nPackets=ID(3);

            % acknowledge that we got the total number of image packets
            pnet(udpo,'write',typecast(int32(nPackets),'uint8'));              % Write to write buffer
            pnet(udpo,'writepacket',remotehost,remoteport);   % Send buffer as UDP packet

            t1=tic;
            counter=0;
            longbuff=uint8(zeros(1,PACK_SIZE*nPackets));
            wrong_packets=false;
            while (counter < nPackets)
                % we took long time
                if (toc(t1)>2)
                    disp('took long time')
                    wrong_packets=true;
                    break;
                end
                % read one image packet

                len=pnet(udpo,'readpacket','noblock');
                %data=pnet(udpo,'read', 5000, 'uint8', 'swap', 'noblock');
                data=pnet(udpo,'read',5000,'uint8');

                if len ~= PACK_SIZE
                    %disp('wrong pack_size. continue to next packet')
                    continue;
                end

                longbuff((counter*PACK_SIZE+1):(counter*PACK_SIZE+PACK_SIZE))=data;
                counter=counter+1;

                % acknowledge that we got one image packet
                pnet(udpo,'write',typecast(int32(999),'uint8'));% Write to write buffer
                pnet(udpo,'writepacket',remotehost,remoteport);   % Send buffer as UDP packet

            end

            if (nPackets==counter)
                [img]=decodeUDPimg(longbuff,counter*PACK_SIZE);
                imshow(img)
                %disp('got complete image...')
                wrong_packets=true;
            end
        else
            if toc(t0)>2
                % we took long time to receive packets
                break;
            end
        end
        %pause(0.01)
    end
    clc
    disp('Timeout. Received no image.')
    pnet(udpo,'close');
    pnet('closeall')
    disp('closing and clearing..')
    clear
    close all
catch
    clc
    close all
    pnet(udpo,'close');
    pnet('closeall')
    disp('closing and clearing..')
    clear
end
