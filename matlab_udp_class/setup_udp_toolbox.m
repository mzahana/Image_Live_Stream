%% setup tcp_udp_ip toolbox

if ispc
    mex -O pnet.c ws2_32.lib -DWIN32
end

if ismac
    mex -O pnet.c
end