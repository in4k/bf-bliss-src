if "%1"=="Release" goto Release
if "%1"=="Compress" goto Release
if "%1"=="DebugShader" goto Debug
echo "Not happening, %1."
goto End

:Debug
fxc /Zi /E"VS" /Od		/Fo"%3%1\VS.cso"	/Fd"%3%1\VS.pdb"	/T vs_5_0 /nologo %2
fxc /Zi /E"VSCB" /Od	/Fo"%3%1\VSCB.cso"	/Fd"%3%1\VSCB.pdb"	/T vs_5_0 /nologo %2
fxc /Zi /E"VSP" /Od		/Fo"%3%1\VSP.cso"	/Fd"%3%1\VSP.pdb"	/T vs_5_0 /nologo %2 
fxc /Zi /E"VSC" /Od		/Fo"%3%1\VSC.cso"	/Fd"%3%1\VSC.pdb"	/T vs_5_0 /nologo %2 
fxc /Zi /E"VSV" /Od		/Fo"%3%1\VSV.cso"	/Fd"%3%1\VSV.pdb"	/T vs_5_0 /nologo %2 
fxc /Zi /E"HS" /Od		/Fo"%3%1\HS.cso"	/Fd"%3%1\HS.pdb"	/T hs_5_0 /nologo %2 
fxc /Zi /E"HSP" /Od		/Fo"%3%1\HSP.cso"	/Fd"%3%1\HSP.pdb"	/T hs_5_0 /nologo %2 
fxc /Zi /E"DS" /Od		/Fo"%3%1\DS.cso"	/Fd"%3%1\DS.pdb"	/T ds_5_0 /nologo %2 
fxc /Zi /E"DSPN" /Od	/Fo"%3%1\DSPN.cso"	/Fd"%3%1\DSPN.pdb"	/T ds_5_0 /nologo %2 
fxc /Zi /E"DSPS" /Od	/Fo"%3%1\DSPS.cso"	/Fd"%3%1\DSPS.pdb"	/T ds_5_0 /nologo %2 
fxc /Zi /E"DSFN" /Od	/Fo"%3%1\DSFN.cso"	/Fd"%3%1\DSFN.pdb"	/T ds_5_0 /nologo %2 
fxc /Zi /E"DSFS" /Od	/Fo"%3%1\DSFS.cso"	/Fd"%3%1\DSFS.pdb"	/T ds_5_0 /nologo %2 
fxc /Zi /E"VST" /Od		/Fo"%3%1\VST.cso"	/Fd"%3%1\VST.pdb"	/T vs_5_0 /nologo %2 
fxc /Zi /E"VSTP" /Od	/Fo"%3%1\VSTP.cso"	/Fd"%3%1\VSTP.pdb"	/T vs_5_0 /nologo %2 
fxc /Zi /E"PS" /Od		/Fo"%3%1\PS.cso"	/Fd"%3%1\PS.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSH" /Od		/Fo"%3%1\PSH.cso"	/Fd"%3%1\PSH.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSTN" /Od	/Fo"%3%1\PSTN.cso"	/Fd"%3%1\PSTN.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"FSTN" /Od	/Fo"%3%1\FSTN.cso"	/Fd"%3%1\FSTN.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSTS" /Od	/Fo"%3%1\PSTS.cso"	/Fd"%3%1\PSTS.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSTD" /Od	/Fo"%3%1\PSTD.cso"	/Fd"%3%1\PSTD.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSDD" /Od	/Fo"%3%1\PSDD.cso"	/Fd"%3%1\PSDD.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSP" /Od		/Fo"%3%1\PSP.cso"	/Fd"%3%1\PSP.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSC" /Od		/Fo"%3%1\PSC.cso"	/Fd"%3%1\PSC.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"PSPS" /Od	/Fo"%3%1\PSPS.cso"	/Fd"%3%1\PSPS.pdb"	/T ps_5_0 /nologo %2 
fxc /Zi /E"GSP" /Od		/Fo"%3%1\GSP.cso"	/Fd"%3%1\GSP.pdb"	/T gs_5_0 /nologo %2 
fxc /Zi /E"GSM" /Od		/Fo"%3%1\GSM.cso"	/Fd"%3%1\GSM.pdb"	/T gs_5_0 /nologo %2 
fxc /Zi /E"GSR" /Od		/Fo"%3%1\GSR.cso"	/Fd"%3%1\GSR.pdb"	/T gs_5_0 /nologo %2 
fxc /Zi /E"GSQ" /Od		/Fo"%3%1\GSQ.cso"	/Fd"%3%1\GSQ.pdb"	/T gs_5_0 /nologo %2 
fxc /Zi /E"GST" /Od		/Fo"%3%1\GST.cso"	/Fd"%3%1\GST.pdb"	/T gs_5_0 /nologo %2 
fxc /Zi /E"VSM" /Od		/Fo"%3%1\VSM.cso"	/Fd"%3%1\VSM.pdb"	/T vs_5_0 /nologo %2 
goto End

:Release
..\ShaderMinifier\shader_minifier.exe -o %3Engine\Shader.h --hlsl --no-renaming-list VS,VSV,VSC,VSCB,VSP,HS,HSP,DS,DSFN,DSFS,DSPN,DSPS,VST,PSTS,PSTN,VSTP,PS,PSC,PSH,PST,PSP,PSPS,GSP,GSM,GSR,GSQ,GST,VSM,FSTN,PSTD,PSDD,CPCFT,CPCFP %2
goto End

:End
