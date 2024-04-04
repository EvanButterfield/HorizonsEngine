@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

set VSOptions=/T vs_5_0 /E VS
set PSOptions=/T ps_5_0 /E PS
set Options=/Ges /nologo /Od /WX /Zi /Zpr

echo "Compiling New Horizons shaders"
del *.fxc > NUL 2>&1
fxc %VSOptions% /Fo shader_vs.fxc %Options% w:\new-horizons\code\shaders.hlsl
fxc %PSOptions% /Fo shader_ps.fxc %Options% w:\new-horizons\code\shaders.hlsl
echo "Done compiling New Horizons shaders"

popd