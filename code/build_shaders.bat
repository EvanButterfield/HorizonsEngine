@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

set VSOptions=/T vs_5_0 /E VS
set PSOptions=/T ps_5_0 /E PS
set Options=/Ges /nologo /Od /WX /Zi /Zpr /Qstrip_reflect /Qstrip_debug /Qstrip_priv

echo "Compiling New Horizons shaders"
fxc %VSOptions% /Fo shader_vs.fxc %Options% w:\new-horizons\code\shaders.hlsl
fxc %PSOptions% /Fo shader_ps.fxc %Options% w:\new-horizons\code\shaders.hlsl

fxc %VSOptions% /Fo fancy_shader_vs.fxc %Options% w:\new-horizons\code\fancy_shaders.hlsl
fxc %PSOptions% /Fo fancy_shader_ps.fxc %Options% w:\new-horizons\code\fancy_shaders.hlsl
echo "Done compiling New Horizons shaders"

popd