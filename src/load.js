/*
var style = document.createElement('style');
style.innerHTML = `
#canvas {
	    position: absolute;
	    top: 0px;
	    left: 0px;
	    margin: 0px;
	    width: 100%;
	    height: 100%;
	    overflow: hidden;
	    display: block;
}`;
document.head.appendChild(style);
*/

if(typeof window === 'object')
{
Module['arguments'] = window.location.search.substr(1).trim().split('&');
// If no args were passed arguments = [''], in which case kill the single empty string.
if (Module['arguments'][0])
{
  Module.preRun.push( function() {
         FS.createPreloadedFile('/tmp/','file.pzw',Module['arguments'][0],true,true);
         Module['arguments'][0]='/tmp/file.pzw';
  });
}
else{
  Module['arguments'] = [];
}
}
