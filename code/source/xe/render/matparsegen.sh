
bison -o MaterialParser.c -d -l -p matparse_ material_parser.y 
flex -oMaterialLexer.c -Pmatlex_  material_lexer.l