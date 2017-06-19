
var logCounter = 0

function log(){
    if (logCounter >= 50) return
    logCounter++
    
    console.log.apply(console, arguments)
}
