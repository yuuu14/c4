assignment-expression:
	conditional-expression
	unary-expression = assignment-expression
	
primary-expression:
	identifier
	constant
	string-literal
	( assignment-expression )


postfix-expression:
	primary-expression
	postfix-expression [ assignment-expression ]
	postfix-expression ( argument-expression-list opt )
	postfix-expression . identifier
	postfix-expression -> identifier
	postfix-expression ++
	postfix-expression --

argument-expression-list:
	assignment-expression
	argument-expression-list , assignment-expression

unary-expression:
	postfix-expression
	unary-operator unary-expression
	sizeof unary-expression
	sizeof ( type-name )

unary-operator:
	& *  -  !

multiplicative-expression:
	unary-expression
	multiplicative-expression * unary-expression

additive-expression:
	multiplicative-expression
	additive-expression + multiplicative-expression
	additive-expression - multiplicative-expression


relational-expression:
	additive-expression
	relational-expression < additive-expression

equality-expression:
	relational-expression
	equality-expression == relational-expression
	equality-expression != relational-expression


logical-AND-expression:
	equality-expression
	logical-AND-expression && equality-expression

logical-OR-expression:
	logical-AND-expression
	logical-OR-expression || logical-AND-expression

conditional-expression:
	logical-OR-expression
	logical-OR-expression ? assignment-expression : conditional-expression
