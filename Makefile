all: parser.tab lexer.l.c
	g++  -o compiler parser.tab.cpp lexer.l.c Sources/Assembly.cpp Sources/Commands.cpp Sources/Conditions.cpp Sources/Environment.cpp Sources/Expressions.cpp Sources/Identifiers.cpp Sources/Logger.cpp Sources/Nodes.cpp Sources/Tester.cpp Sources/Utility.cpp Sources/Value.cpp

lexer.l.c: lexer.l
	flex -o lexer.l.c lexer.l

parser.tab: parser.ypp
	bison -d parser.ypp
