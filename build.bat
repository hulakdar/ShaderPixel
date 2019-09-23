devenv ShaderPixel.sln /BUILD Release
goto END

@echo off

set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

REM TODO - can we just build both with one exe?

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

del *.pdb > NUL 2> NUL
REM Optimization switches /O2 /Oi /fp:fast
cl %CommonCompilerFlags% .\ShaderPixel\src\ShaderPixel.cpp -FmShaderPixel.map -LD %CommonLinkerFlags% -PDB:ShaderPixel_%random%.pdb -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender -o .\bin\
popd

:END
