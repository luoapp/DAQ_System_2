function hdr=tripod_header(fid);
%[HDR]=TRIPOD_HEADER(FID) gets the header information from the file.
%
%By Aijun Song
%Last updated on Apr.7, 2008
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
count=0; header_in_bytes=320;
%UDEL ID
[tempt, ct]=fread(fid, 8,'uchar');      
chk_read(ct); count=count+ct;
hdr.id_str= sprintf('%s',char(tempt)); 
%YEAR
[tempt,ct]= fread(fid, 4,'uchar');
chk_read(ct); count=count+ct;
hdr.year=str2num(sprintf('%s',char(tempt)));
%MONTH
[tempt,ct]= fread(fid, 2,'uchar');
chk_read(ct); count=count+ct;
hdr.month=str2num(sprintf('%s',char(tempt)));
%DAY
[tempt,ct]= fread(fid, 2,'uchar');
chk_read(ct); count=count+ct;
hdr.day=str2num(sprintf('%s',char(tempt)));
%SKIP 'T'
[tempt,ct]= fread(fid, 1,'uchar');
chk_read(ct); count=count+ct;
%HOUR
[tempt,ct]= fread(fid, 2,'uchar');
chk_read(ct); count=count+ct;
hdr.hour=str2num(sprintf('%s',char(tempt)));
%MINUTE
[tempt,ct]= fread(fid, 2,'uchar');
chk_read(ct); count=count+ct;
hdr.minute=str2num(sprintf('%s',char(tempt)));
%SECOND
[tempt,ct]= fread(fid, 19,'uchar');
chk_read(ct); count=count+ct;
tempt_str=sprintf('%s',char(tempt));
tindex=findstr(upper(tempt_str),'GMT');
if isempty(tindex),
    tindex=findstr(upper(tempt_str),'UTC');
end
hdr.second=str2num(tempt_str(1: tindex(1)-1));

%Get the geotime 
hdr.geotime=datenum([hdr.year hdr.month hdr.day hdr.hour hdr.minute hdr.second]);

%STATUS
[tempt,ct]= fread(fid, 11,'uint32');
chk_read(ct); count=count+ct*4;
hdr.status=tempt;

%CTL.CFG 
[tempt,ct]= fread(fid, 3,'int32');
chk_read(ct); count=count+ct*4;
hdr.cfg_version=tempt(1);
hdr.cfg_inchans=tempt(2);
hdr.cfg_outchans=tempt(3);
[tempt,ct]= fread(fid, 2,'float32');
chk_read(ct); count=count+ct*4;
hdr.cfg_masterclock=tempt(1);
%HERE DISCRIPANCY WITH THE DR. NELSON'S REPORT
%HERE DISCRIPANCY WITH THE DR. NELSON'S REPORT

%CTL.ICTL
[tempt,ct]= fread(fid, 2,'float32');
chk_read(ct); count=count+ct*4;
hdr.ictl_vfs=tempt(1);
hdr.ictl_clockrate=tempt(2);

[tempt,ct]= fread(fid, 8,'int32');
chk_read(ct); count=count+ct*4;
hdr.ictl_ndiv=tempt(1);
hdr.ictl_clkmaster=tempt(2);
hdr.ictl_nmode=tempt(3);
hdr.ictl_nrange=tempt(4);
hdr.ictl_nchans=tempt(5);
hdr.ictl_nburst=tempt(6);
hdr.ictl_nthreshold=tempt(7);
hdr.ictl_ndata=tempt(8);

%CTL.OCTL
[tempt,ct]= fread(fid, 2,'float32');
chk_read(ct); count=count+ct*4;
hdr.octl_vfs=tempt(1);
hdr.octl_clockrate=tempt(2);

[tempt,ct]= fread(fid, 8,'int32');
chk_read(ct); count=count+ct*4;
hdr.octl_ndiv=tempt(1);
hdr.octl_clkmaster=tempt(2);
hdr.octl_nmode=tempt(3);
hdr.octl_nrange=tempt(4);
hdr.octl_nchans=tempt(5);
hdr.octl_nburst=tempt(6);
hdr.octl_nthreshold=tempt(7);
hdr.octl_ndata=tempt(8);

%CTL.TRIGMASTER
[tempt,ct]= fread(fid, 1,'int32');
hdr.ctl_trigmaster=tempt;
count=count+ct*4;

%Find the sample_per_block and block_requested
fseek(fid, header_in_bytes-8, 'bof');
[tempt,ct]= fread(fid, 2,'int32');
hdr.sample_per_block=tempt(1);
hdr.block_requested=tempt(2);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%End of file 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%