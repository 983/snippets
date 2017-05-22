
// ultility functions

function identity(value){
    return value
}

function swap(values, i, j){
    var temp = values[i]
    values[i] = values[j]
    values[j] = temp
}

function forEach(values, f){
    var n = values.length
    for (var i = 0; i < n; i++){
        f(values[i])
    }
}

function map(f, values){
    var n = values.length
    var result = new values.constructor(n)
    for (var i = 0; i < n; i++){
        result[i] = f(values[i])
    }
    return result
}

function apply(f, values){
    return f.apply(null, values)
}

function copy(values){
    return map(identity, values)
}

function reverse(values){
    var n = values.length
    for (var i = 0, j = n - 1; i < j; i++, j--){
        swap(values, i, j)
    }
}

function reversed(values){
    var result = copy(values)
    reverse(result)
    return result
}

function concat(){
    var lists = arguments
    
    var result = []
    forEach(lists, function(list){
        list.forEach(function(value){
            result.push(value)
        })
    })
    return result
}

function contains(values, value){
    return values.indexOf(value) !== -1
}

function flatten(values, result){
    if (result === undefined) result = []
    
    values.forEach(function(value){
        if (value.constructor === Array){
            flatten(value, result)
        }else{
            result.push(value)
        }
    })
    
    return result
}

function stripLeft(s){
    var i, n = s.length
    for (i = 0; i < n; i++){
        if (!contains(" \n\t\r", s[i])) break
    }
    return s.substr(i)
}

function filter(values, keep){
    var result = []
    var n = values.length
    for (var i = 0; i < n; i++){
        var value = values[i]
        if (keep(value)){
            result.push(value)
        }
    }
    return result
}

function not(f){
    return function(){
        return !f.apply(null, arguments)
    }
}

function len(values){
    return values.length
}

function nth(i){
    return function(values){
        return values[i]
    }
}

var second = nth(1)

function flatten1(value_and_values){
    var [value, values] = value_and_values
    return concat([value], values)
}







// parser structures

function Token(value, next, data){
    if (data === undefined) data = value
    
    this.value = value
    this.next = next
    this.data = data
}

function ParserResult(token, value, error){
    this.token = token
    this.value = value
    this.error = error
}







// misc parser helper functions

function make_tokens(values){
    if (values.constructor === String){
        values = values.split("")
    }

    var token = null
    
    values = reversed(values)
    
    values.forEach(function(value){
        token = new Token(value, token)
    })
    
    return token
}

function chainTokens(tokens){
    var result = null
    tokens = reversed(tokens)
    tokens.forEach(function(token){
        token.next = result
        result = token
    })
    return result
}




// parser functions

function any(values){
    return function(token){
        if (token && contains(values, token.value)){
            return new ParserResult(token.next, token.data)
        }
        return new ParserResult(token, null, values)
    }
}

function val(value){
    return function(token){
        if (!token || token.value !== value){
            return new ParserResult(token, null, value)
        }
        return new ParserResult(token.next, token.data)
    }
}

function str(s){
    return function(token){
        var n = s.length
        for (var i = 0; i < n; i++){
            if (token.value !== s[i]){
                return new ParserResult(token, null, s.slice(i))
            }
            token = token.next
        }
        return new ParserResult(token, s)
    }
}

function zero_or_more(parser, initialValues){
    return function(token){
        var values = initialValues
        
        if (values === undefined) values = []
        
        while (token){
            // parse token or break
            var result = parser(token)
            if (result.error) break
            token = result.token
            values.push(result.value)
        }
        
        return new ParserResult(token, values)
    }
}

function one_or_more(parser){
    return function(token){
        var values = []
        
        // parse token or return
        var result = parser(token)
        if (result.error) return result
        token = result.token
        values.push(result.value)
        
        return zero_or_more(parser, values)(token)
    }
}

function or(){
    var parsers = arguments
    
    return function(token){
        var errors = []
        
        for (var i = 0; i < parsers.length; i++){
            var parser = parsers[i]
            
            var result = parser(token)
            if (!result.error) return result
            
            errors.push(result.error)
        }
        
        return new ParserResult(token, null, errors)
    }
}

function and(){
    var parsers = arguments
    
    return function(token){
        var values = []
        
        for (var i = 0; i < parsers.length; i++){
            var parser = parsers[i]
            
            var result = parser(token)
            if (result.error) return result
            token = result.token
            values.push(result.value)
        }
        
        return new ParserResult(token, values)
    }
}

function modify(parser, f){
    return function(token){
        var result = parser(token)
        if (result.error) return result
        result.value = f(result.value)
        return result
    }
}

function optional(parser){
    return function(token){
        var result = parser(token)
        if (result.error) return new ParserResult(token, [])
        return result
    }
}

function debug(parser, message){
    return function(token){
        console.log(message)
        return parser(token)
    }
}








// language definition
/*
var language = {
    "+": function(a, b){ return a + b; },
    "-": function(a, b){ return a - b; },
    "*": function(a, b){ return a * b; },
    "/": function(a, b){ return a / b; },
    "%": function(a, b){ return a % b; },
    
    "unary+": function(a){ return +a; },
    "unary-": function(a){ return -a; },
}*/


var language = {
    "+": function(a, b){ return ["+", a, b]; },
    "-": function(a, b){ return ["-", a, b]; },
    "*": function(a, b){ return ["*", a, b]; },
    "/": function(a, b){ return ["/", a, b]; },
    "%": function(a, b){ return ["%", a, b]; },
    
    "unary+": function(a){ return ["unary+", a]; },
    "unary-": function(a){ return ["unary-", a]; },
}




// language definition

var digit = any("0123456789")
var dot = val(".")
var sign = any("+-")

var uint_raw = one_or_more(digit)
var int_raw = and(optional(sign), uint_raw)

var exponent = and(any("eE"), int_raw)

var float0 = and(optional(sign), dot, uint_raw, optional(exponent))
var float1 = and(int_raw, dot, optional(uint_raw), optional(exponent))
var float2 = and(int_raw, exponent)
var float_raw = or(float0, float1, float2)

var int = modify(int_raw, function(value){
    return parseInt(flatten(value).join(""))
})

var float = modify(float_raw, function(value){
    return parseFloat(flatten(value).join(""))
})

var whitespace = any(" \n\t\r")

var identifier_first = any("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")
var identifier_rest  = any("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789")

var identifier_raw = and(identifier_first, zero_or_more(identifier_rest))

var identifier = modify(identifier_raw, function(identifier_first_and_rest){
    var [identifier, rest] = identifier_first_and_rest
    return identifier + rest.join("")
})

function wrap_as_token(parser, name){
    return modify(parser, function(value){
        return new Token(name, null, value)
    })
}

// TODO better name
var keywords = [
    "=",
    ",",
    ";",
    ":",
    "?",
    
    "(", ")",
    "{", "}",
    "<", ">",
    
    "+", "-",
    "*", "/", "%",
    
    "&", "|", "~", "!",
    
    "int",
    "float",
    "vec2",
    "vec3",
    "vec4",
    "return",
]

var tokens = [
    wrap_as_token(float, "float"),
    wrap_as_token(int, "int"),
]

keywords.forEach(function(keyword){
    tokens.push(wrap_as_token(str(keyword), keyword))
})

tokens.push(wrap_as_token(identifier, "identifier"))

var whitespaces = zero_or_more(whitespace)
var any_token = modify(and(whitespaces, apply(or, tokens), whitespaces), second)

var tokenizer = zero_or_more(any_token)










// tokenization

function removeComment(line){
    var i = line.indexOf("//")
    
    if (i === -1) return line
    
    return line.substr(0, i)
}

function isMacro(line){
    return stripLeft(line).startsWith("#")
}

function tokenize(source){
    var lines = source.split("\n")

    lines = map(stripLeft, lines)

    // TODO block comments
    lines = map(removeComment, lines)

    // TODO preprocessor
    lines = filter(lines, not(isMacro))

    lines = filter(lines, len)

    source = lines.join("\n")

    var result = tokenizer(make_tokens(source))

    console.log(result.value.length +  " tokens")
    console.log("next token:", result.token)
    console.log("")
    
    for (var i = 0; i < Math.min(result.value.length, 50); i++){
        console.log(i, result.value[i].data, result.value[i].value)
    }
    console.log("")

    var tokens = chainTokens(result.value)
    
    return tokens
}
/*
var fs = require("fs")
var source = fs.readFileSync("source2.glsl").toString()
*/

var source = `

float foo(float a, float b){
    float c = a + b;
    return a + b * c;
}

vec4 bar(vec4 a, vec4 b){
    return a + b;
}

`

var tokens = tokenize(source)


















// parsing

var identifier = val("identifier")


var type = apply(or, map(val, [
    "int",
    "float",
    "vec2",
    "vec3",
    "vec4",
]))

var parameter = and(type, identifier)
var comma_parameter = modify(and(val(','), parameter), second)
var parameters_raw = and(parameter, zero_or_more(comma_parameter))
var parameters = modify(parameters_raw, flatten1)

parameters = optional(parameters)

var term = or(float, int, identifier)
var unary_raw = and(zero_or_more(sign), term)

var unary = modify(unary_raw, function(signs_and_term){
    var [signs, result] = signs_and_term
    signs.forEach(function(sign){
        result = language["unary" + sign](result)
    })
    return result
})

var mul_raw = and(unary, zero_or_more(and(any("*%/"), unary)))

var mul = modify(mul_raw, function(value){
    var result = value[0]
    value[1].forEach(function(op_and_rhs){
        var [op, rhs] = op_and_rhs
        result = language[op](result, rhs)
    })
    return result
})

var add_raw = and(mul, zero_or_more(and(sign, mul)))

var add = modify(add_raw, function(value){
    var result = value[0]
    value[1].forEach(function(op_and_rhs){
        var [op, rhs] = op_and_rhs
        result = language[op](result, rhs)
    })
    return result
})

var expression = add

var statement0 = and(type, identifier, val("="), expression)
var statement1 = and(val("return"), optional(expression))
// TODO more statements
var statement = and(or(statement0, statement1), val(';'))
var statements = zero_or_more(statement)

var function_declaration_header = and(type, identifier, val('('), parameters, val(')'))
var function_declaration_body = and(val("{"), statements, val("}"))
var function_declaration_raw = and(function_declaration_header, function_declaration_body)

var function_declaration = modify(function_declaration_raw, function(header_and_body){
    var [header, body] = header_and_body
    var [type, name, _, parameters, _] = header
    var [_, statements, _] = body
    return ["function", type, name, parameters, statements]
})

var parser = and(function_declaration, function_declaration)

var result = parser(tokens)
console.log("next token:", result.token)
console.log("")
console.log("result.value:")
console.log(JSON.stringify(result.value))
console.log("")
if (result.error){
    console.log("error:", result.error)
    console.log("")
}

var remaining = []
var token = result.token
for (var i = 0; i < 100; i++){
    if (token === null) break
    remaining.push(token.value)
    token = token.next
}
console.log("remaining:")
console.log(remaining.join(" "))
console.log("")
























// tests

function tryParse(parser, values, expected){
    var token = make_tokens(values)

    var result = parser(token)
    
    if (result.error){
        console.log("ERROR:")
        console.log("")
        console.log("expected:")
        console.log(result.error)
        console.log("")
        return
    }

    if (result.token !== null){
        console.log("ERROR: did not parse all tokens")
        console.log(result.token)
        console.log("")
        return
    }

    if (JSON.stringify(result.value) !== JSON.stringify(expected)){
        console.log("ERROR:")
        console.log("unexpected result")
        console.log("")
        console.log("expected:")
        console.log(expected)
        console.log("")
        console.log("but got:")
        console.log(result.value)
        console.log("")
    }
}

tryParse(int, "+1234".split(""), 1234)
tryParse(int, "-1234".split(""), -1234)
tryParse(int, "1234".split(""), 1234)
tryParse(float, ".1E-3".split(""), 0.0001)
tryParse(float, ".1".split(""), 0.1)
tryParse(float, "12.34e2".split(""), 1234)
tryParse(float, "12.34".split(""), 12.34)
tryParse(float, "1234e-4".split(""), 0.1234)
tryParse(float, "123.".split(""), 123.0)
tryParse(float, "0.".split(""), 0.0)
tryParse(float, "-.1".split(""), -0.1)
tryParse(float, "-123e2".split(""), -12300)
