javascript:(function(){function g(c){var a=new XMLHttpRequest;a.open("POST","http://127.0.0.1:5000",!0);a.onload=function(a){console.log(a)};c=new Blob([c],{type:"application/octet-stream"});a.send(c)}for(var e={},f=document.getElementsByTagName("img"),d=0;d<f.length;d++){var a=f[d].src;if(a&&!a.startsWith("data:")&&!(a in e)){e[a]=!0;var b=new XMLHttpRequest;b.url=a;b.responseType="arraybuffer";b.open("GET",a);b.onloadend=function(a){console.log("Downloading "+b.url);a=new Uint8Array(a.target.response);g(a)}; b.send()}}})();
