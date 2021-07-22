/** helpers */
function get_id(id) {
    return document.getElementById(id);
}

/** C interface wrappers */
function lxrad_scale_up() {
    return Module.ccall('lxrad_scale_up', null, null, null);
}

function lxrad_scale_down() {
    return Module.ccall('lxrad_scale_down', null, null, null);
}

/** emscripten wrapper page Javascript functions **/

var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var loaded = false;
var Module = {
    preRun: [],
    postRun: [
        function() {
            initDragAndDrop();
        },
    ],
    print: (function() {
        return function(text) {
            text = Array.prototype.slice.call(arguments).join(' ');
            console.log(text);
        };
    })(),
    printErr: function(text) {
        text = Array.prototype.slice.call(arguments).join(' ');
        console.error(text);
    },
    canvas: (function() {
        var canvas = get_id('canvas');
        canvas.addEventListener("webglcontextlost", function(e) { alert('FIXME: WebGL context lost, please reload the page'); e.preventDefault(); }, false);
        return canvas;
    })(),
    setStatus: function(text) {
         console.log("status: " + text);
          if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
          if (text === Module.setStatus.last.text) return;
          var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
          var now = Date.now();
          if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
          Module.setStatus.last.time = now;
          Module.setStatus.last.text = text;
          if (m) {
            text = m[1];
            progressElement.value = parseInt(m[2])*100;
            progressElement.max = parseInt(m[4])*100;
            progressElement.hidden = false;
            spinnerElement.hidden = false;
          } else {
            progressElement.value = null;
            progressElement.max = null;
            progressElement.hidden = true;
            if (!text)
            { 
             spinnerElement.style.display = 'none';
             var canvas = get_id('canvas');
             canvas.style.position= 'absolute';
             canvas.style.top='0px';
             canvas.style.left='0px';            
	     canvas.style.margin='0px';            
             canvas.style.width='100%';
             canvas.style.height='100%';
             canvas.style.overflow='hidden';
             canvas.style.display= 'block';   
            }
          }
          statusElement.innerHTML = text;
        },
        totalDependencies: 0,
        monitorRunDependencies: function(left) {
          this.totalDependencies = Math.max(this.totalDependencies, left);
          Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
        }
      };
      Module.setStatus('Downloading...');
      window.onerror = function(event) {
        // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
        Module.setStatus('Exception thrown, see JavaScript console');
        spinnerElement.style.display = 'none';
        get_id('canvas').style.top= '80px';
        Module.setStatus = function(text) {
          if (text) Module.printErr('[post-exception status] ' + text);
        };
      };


function callAsEventHandler(func_name) {
    // this is some hackery to make the browser module believe that it
    // is running in an event handler
    var eventHandler = { allowsDeferredCalls: true };
    ++JSEvents.inEventHandler;
    JSEvents.currentEventHandler = eventHandler;
    Module.cwrap(func_name)()
    --JSEvents.inEventHandler;
}


// drag-n-drop functions
function initDragAndDrop() {
    // add a drag'n'drop handler to the WebGL canvas
    get_id('load_panel_container').addEventListener('dragenter', load_dragenter, false);
    get_id('load_panel_container').addEventListener('dragleave', load_dragleave, false);
    get_id('load_panel_container').addEventListener('dragover', load_dragover, false);
    get_id('load_panel_container').addEventListener('drop', load_drop, false);
}

function load_dragenter(e) {
    e.stopPropagation();
    e.preventDefault();
}

function load_dragleave(e) {
    e.stopPropagation();
    e.preventDefault();
    get_id('load_panel_container').classList.remove('highlight');
}

function load_dragover(e) {
    get_id('load_panel_container').classList.add('highlight');
    e.stopPropagation();
    e.preventDefault();
}

function load_drop(e) {
    e.stopPropagation();
    e.preventDefault();
    get_id('load_panel_container').classList.remove('highlight');
    toggle_load_panel();
    load_file(e.dataTransfer.files);
}

// load a file from the filepicker 
function load_file(files) {
    if (files.length > 0) {
        var file = files[0];
        console.log('--- load file:');
        console.log('  name: ' + file.name);
        console.log('  type: ' + file.type);
        console.log('  size: ' + file.size);
        
        // load the file content (ignore big files)
        if (file.size < 256000) {
            var reader = new FileReader();
            reader.onload = function(loadEvent) {
                console.log('file loaded!')
                var content = loadEvent.target.result;
                if (content) {
                    console.log('content length: ' + content.byteLength);
                    var uint8Array = new Uint8Array(content);
                    FS.writeFile('/tmp/'+file.name , uint8Array); 
                    var res = Module.ccall('file_ready',  // C function name
                        null,
                        ['string'],  // name
                        [file.name]);
                    if (res == 0) {
                        console.warn('file_ready failed!');
                    } 
                }
                else {
                    console.warn('load result empty!');
                }
            };
            reader.readAsArrayBuffer(file);
        }
        else {
            console.warn('ignoring dropped file because it is too big')
        }
    }
}

// toggle the nav bar
function nav_toggle() {
    get_id('nav').classList.toggle('toggle');
}

// show/hide panels

function toggle_load_panel() {
    get_id('load_panel').classList.toggle('hidden');
    get_id('quickload_panel').classList.add('hidden');
    get_id('systems_panel').classList.add('hidden');
    get_id('load_panel_container').classList.remove('highlight');
}


function ui_open_filepicker() {
    get_id('filepicker').click();
}

function togglePlay() {
    let classes = get_id('play_btn').classList;
    classes.toggle("paused");
    let result = classes.contains("paused");
    if (result) {
      Module.ccall('SimRun', null , ['int'], [0]);
    } else {
      Module.ccall('SimRun', null , ['int'], [1]);
    }

}

