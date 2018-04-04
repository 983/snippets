//https://medium.com/@umpox/be-careful-what-you-copy-invisibly-inserting-usernames-into-text-with-zero-width-characters-18b4e6f17b66
var TextDecoder = require('./TextDecoderLite').TextDecoderLite;
var TextEncoder = require('./TextDecoderLite').TextEncoderLite;

function textToBytes(s){
    var encoder = new TextEncoder("utf-8");
    return encoder.encode(s);
}

function bytesToBits(bytes){
    var n = bytes.length;
    var bits = new Uint8Array(n*8);
    for (var i = 0; i < bytes.length; i++){
        var byte = bytes[i];
        
        for (var j = 0; j < 8; j++){
            bits[i*8 + j] = (byte >> j) & 1;
        }
    }
    return bits;
}

var bits = bytesToBits(textToBytes("asdf"));

var encoded = [];

var alphabet = [
    0x200b,
    //0x200c,
    //0x200d,
    //0xfeff,
    0x2062,
    0x2063,
    0x2064,
].map(String.fromCharCode)

console.log("alphabet:");
console.log(alphabet);

// alphabet can encode up to 2 bits of information
var alphabetBitLength = 2;

for (var i = 0; i < bits.length; i += alphabetBitLength){
    var index = 0;
    for (var j = 0; j < alphabetBitLength; j++){
        index |= bits[i + j] << j;
    }
    
    encoded.push(alphabet[index]);
}

encoded = encoded.join('')

//console.log(encoded);
//console.log(encoded.length, "chars");

/*
function charToBinary(c){
    return c.charCodeAt(0).toString(2).padStart(8, "0")
}

function textToBinary(text){
    return text.split('').map(charToBinary).join(" ")
}

function binaryToZeroWidth(binary){
    function f(x){
        var num = parseInt(x, 10);
        if (num === 1){
            return "a"
        }else if (num === 0){
            return "b"
        }else{
            return "c"
        }
    }
    return binary.split('').map(f).join('d')
}

console.log(binaryToZeroWidth(textToBinary("asdf")))
*/

if (0){
    var canvas = document.createElement("canvas");
    var context = canvas.getContext("2d");
    context.font = "12pt arial";

    var s = [];
    for (var i = 0; i < 0xffff; i++){
      var text = String.fromCharCode(i);

      var metrics = context.measureText(text);

      if (metrics.width === 0){
        s.push(text);
        //console.log(i.toString(16))
      }
    }
    console.log(s.length);
    console.log("before")
    console.log(s.join(''))
    console.log("after")
}
