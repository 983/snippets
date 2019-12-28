var inputElement = document.getElementById("fileinput");

function handleFile(file){
    var reader = new FileReader();
    
    var offset = 0;
    var chunkSize = 5;
    var counter = 0;
    
    function readChunk(){
        //if (offset >= file.size) return;
        counter++;
        if (counter > 10) return;
        
        console.log("offset:", offset);
        
        var chunk = file.slice(offset, offset + chunkSize);
        
        reader.readAsArrayBuffer(chunk);
    }
    
    reader.onload = function(){
        var data = new Uint8Array(reader.result);
        
        console.log(data);
        
        offset = Math.random() * file.size | 0;
        
        readChunk();
    }
    
    readChunk();
}

inputElement.onchange = function(){
    for (var i = 0; i < this.files.length; i++){
        var file = this.files[i];
        
        handleFile(file);
    }
}
