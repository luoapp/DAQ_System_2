function [output, hdr, hdr2, status]=tripod_read(fname, start_samp, num_samp, sel_chs);
%
%
%By Aijun Song
%Apr 7, 2008
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%SET FILE READING PARAMETERS%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
header_in_bytes=320;
bytes_per_sample=2;
fid = fopen(fname,'rb','ieee-le');
if fid==-1,
    disp('[TRIPOD_READ] Error: Cannot read file.');
    output=[]; hdr=[]; hdr2=[]; status=-1;
    return;
end

%%%%GET RECORD HEADER (TRAILER)
[hdr] = tripod_header(fid);    
nch =hdr.ictl_nchans;                       %total channel number  
if nargin==1,
    start_samp=1;   
    num_samp=[];
    sel_chs=[1:nch];
elseif nargin==3,
    sel_chs=[1:nch];
end
if any(sel_chs>nch)
    disp('[TRIPOD_READ] Error: Input channel numbers exceed the limit in data.');
    output=[]; hdr=[]; hdr2=[]; status=-2;
    return;
end

fseek(fid, 0, 'eof');
position = ftell(fid);
ttl_samp=round((position-2*header_in_bytes)/(bytes_per_sample*nch));

if ttl_samp~=hdr.block_requested*hdr.sample_per_block/nch
    hdr
    ttl_samp
    disp('[TRIPOD_READ] Warning: Data samples may be missing.');
    status=-3;
end

if isempty(num_samp),
    num_samp=ttl_samp-start_samp+1;
end

%%%READ 1 RECORD OF DATA, all channels
fseek(fid, header_in_bytes, 'bof');
fseek(fid, (start_samp-1)*nch*bytes_per_sample, 'cof');

%%%%Data samples are in 16 bits
[all_data, count]=fread(fid, [nch, num_samp],'uint16'); 
if (count <= 0)
    output=[]; hdr=[]; hdr2=[]; status=-4;
    disp('[TRIPOD_READ] Error: No data read in.');
    return;
end

fseek(fid, -header_in_bytes, 'eof');
[hdr2] = tripod_header(fid);    
fclose(fid);

%Get the specified channel data
cap_num=hex2dec('10000');
all_data=all_data(sel_chs,:);
%If the data samples are in 32 bits, strip off the higher 16 bits
%all_data=mod(all_data, cap_num);

%Convert data to volts
%hdr.ictl_vfs is VOLTS FULL SCALE
%The data is in OFFSET BINARY mode.
grid_value=hdr.ictl_vfs/cap_num;
output=(all_data-hex2dec('8000'))*grid_value;

%Make each column as a channel.
output=output.';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%END OF FILE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%