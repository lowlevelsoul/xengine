
bison -o MaterialParser.cpp -d -l -p matparse_ material_parser.y 
flex -oMaterialLexer.cpp -Pmatlex_  material_lexer.l