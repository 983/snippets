function downloadURL(url, filename){
    if (filename === undefined) filename = "download";
    
    var a = document.createElement("a");
    a.href = url;
    a.download = filename;
    a.style = "display: none";
    document.body.appendChild(a);
    a.click();
    a.remove();
}

function downloadBytes(bytes, filename, mimeType){
    if (mimeType === undefined) mimeType = "application/octet-stream";
    var blob = new Blob([bytes], {type: mimeType});
    var url = window.URL.createObjectURL(blob);
    downloadURL(url, filename);
    setTimeout(function(){
        window.URL.revokeObjectURL(url);
    }, 0);
}

async function encrypt(plaintext, key){
    var keyObj = await window.crypto.subtle.importKey("raw", key, "AES-GCM", true, ["encrypt", "decrypt"]);

    var iv = window.crypto.getRandomValues(new Uint8Array(12));

    var resultBuffer = await window.crypto.subtle.encrypt({name: "AES-GCM", iv: iv}, keyObj, plaintext);

    var encrypted = new Uint8Array(iv.length + resultBuffer.byteLength);
    
    encrypted.set(iv, 0);
    encrypted.set(new Uint8Array(resultBuffer), 12);

    return encrypted;
}

function hexlify(bytes){
    var alphabet = "0123456789abcdef";
    var n = bytes.length;
    var result = new Array(2 * n);
    for (var i = 0; i < n; i++){
        var b = bytes[i];
        var hi = b >> 4;
        var lo = b & 0xf;
        result[i * 2 + 0] = alphabet[hi];
        result[i * 2 + 1] = alphabet[lo];
    }
    return result.join("");
}

function unhexlify(text){
    if (text.length % 2 !== 0){
        throw new Error("Expected even text length");
    }
    
    var n = text.length >> 1;
    
    var bytes = new Uint8Array(n);
    
    var char2int = {};
    var alphabet = "0123456789abcdef";
    
    for (var i = 0; i < alphabet.length; i++){
        char2int[alphabet[i]] = i;
    }
    
    for (var i = 0; i < n; i++){
        var hi = char2int[text[i * 2 + 0]];
        var lo = char2int[text[i * 2 + 1]];
        
        bytes[i] = (hi << 4) | lo;
    }
    
    return bytes;
}

async function decrypt(encrypted, key){
    var keyObj = await window.crypto.subtle.importKey("raw", key, "AES-GCM", true, ["encrypt", "decrypt"]);
    
    var iv = encrypted.slice(0, 12);
    
    encrypted = encrypted.slice(12);

    var resultBuffer = await window.crypto.subtle.decrypt({name: "AES-GCM", iv: iv}, keyObj, encrypted);
    
    var decrypted = new Uint8Array(resultBuffer);
    
    return decrypted;
}

function handleFile(file, callback){
    var reader = new FileReader();
    
    reader.onload = function(){
        var data = new Uint8Array(reader.result);
        
        callback(data);
    }
    
    reader.readAsArrayBuffer(file);
}

document.getElementById("decryptFile").onchange = function(){
    for (var i = 0; i < this.files.length; i++){
        handleFile(this.files[i], function(encrypted){
            var password = document.getElementById("decryptPassword").value;
            
            var key = unhexlify(password);
            
            decrypt(encrypted, key).then(function(decrypted){
                downloadBytes(decrypted, "decrypted");
            })
        });
    }
}

document.getElementById("encryptFile").onchange = function(){
    for (var i = 0; i < this.files.length; i++){
        handleFile(this.files[i], function(plaintext){
            var key = window.crypto.getRandomValues(new Uint8Array(32));
            
            var password = hexlify(key);
            
            document.getElementById("encryptPassword").value = password;
            
            encrypt(plaintext, key).then(function(encrypted){
                downloadBytes(encrypted, "encrypted");
            })
        });
    }
}
