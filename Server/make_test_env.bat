pushd %~dp0
mkdir auth
mkdir game\log
mkdir auth\log
mkdir db\log
mkdir game\locale
copy libdevil\win32\bin\* game
copy libdevil\win32\bin\* auth
copy game\game_d.exe auth\auth_d.exe
cd db
mklink item_proto.txt ..\..\..\..\dist\ymir\Test\Server\item_proto.txt
mklink mob_proto.txt ..\..\..\..\dist\ymir\Test\Server\mob_proto.txt
mklink item_proto_test.txt ..\..\..\..\dist\ymir\Test\Server\item_proto_test.txt
mklink mob_proto_test.txt ..\..\..\..\dist\ymir\Test\Server\mob_proto_test.txt
cd ..
cd auth
mklink /D package ..\..\..\..\dist\ymir\Test\Server\package
mklink /D panama ..\..\..\..\dist\ymir\Test\Server\panama
cd ..
cd game
mklink /D data ..\..\..\..\dist\ymir\Test\Server\data
mklink /D package ..\..\..\..\dist\ymir\Test\Server\package
mklink /D panama ..\..\..\..\dist\ymir\Test\Server\panama
cd locale
mklink /D ymir ..\..\..\..\..\dist\ymir\Test\Server\locale\ymir
mklink /D canada ..\..\..\..\..\dist\ca\Test\Server\locale\canada
mklink /D brazil ..\..\..\..\..\dist\br\Test\Server\locale\brazil
mklink /D taiwan ..\..\..\..\..\dist\tw\Test\Server\locale\taiwan
mklink /D NEWCIBN ..\..\..\..\..\dist\NEWCIBN\Test\Server\locale\NEWCIBN
mklink /D we_korea ..\..\..\..\..\dist\we_korea\Test\Server\locale\we_korea
cd ..
cd ..
pause
