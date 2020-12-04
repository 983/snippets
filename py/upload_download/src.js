(function(){
function send(data){
    var r = new XMLHttpRequest();
    r.open("POST", "http://127.0.0.1:5000", true);
    r.onload = function(e){
        console.log(e);
    };
    var blob = new Blob([data], {type: 'application/octet-stream'});
    r.send(blob);
}

var seen = {};
var images = document.getElementsByTagName("img");
for (var i = 0; i < images.length; i++){
    var url = images[i].src;

    if (!url || url.startsWith("data:")) continue;
    if (url in seen) continue
    seen[url] = true;
    var r = new XMLHttpRequest();
    r.url = url;
    r.responseType = 'arraybuffer';
    r.open('GET', url);
    r.onloadend = function(e){
        console.log("Downloading " + r.url);
        var data = new Uint8Array(e.target.response);
        send(data);
    };
    r.send();
}
})();
