
rem set bison=W:\sdk\win_flex_bison\win_bison.exe
rem set flex=W:\sdk\win_flex_bison\win_flex.exe
set bison=D:\Dev\tools\winflexbison\win_bison.exe
set flex=D:\Dev\tools\winflexbison\win_flex.exe
%bison% -o RttiParser.cpp -d -l -p rtti_ rtti_parser.y 
%flex% -oRttiLexer.cpp -Prttilex_  rtti_lexer.l