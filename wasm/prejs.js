let inBrowserWeb = typeof window !== "undefined";
let inBrowserWorker = typeof importScripts !== "undefined";
let inBrowser = inBrowserWeb || inBrowserWorker;
Module["print"] = (msg) => {
  if(inBrowser) console.log(msg);
  else print(msg);
};
Module["printWarn"] = (msg) => {
  if(inBrowser) console.warn(msg);
  else print(msg);
};
Module["printErr"] = (msg) => {
  if(inBrowser) console.error(msg);
  else print(msg);
};
if (typeof locateWASMFile !== "undefined") {
  Module["locateFile"] = locateWASMFile;
}

function compileNoStream(fetchPromise, info, receiveInstance) {
  let compileStart;
  fetchPromise
    .then((resp) => resp.arrayBuffer())
    .then((...args) => {
      compileStart = performance.now();
      return Promise.resolve(...args);
    })
    .then((bytes) => WebAssembly.instantiate(bytes, info))
    .then((obj) => {
      Module["compileTime"] = performance.now() - compileStart;
      receiveInstance(obj.instance, obj.module);
    })
    .catch((reas) => {
      Module["printErr"]("Failed to compile: " + reas)
    });
}

Module["instantiateWasm"] = (info, receiveInstance) => {
  if (inBrowser) {
    const fetchPromise = fetch(wasmBinaryFile, { credentials: "same-origin" });

    if(typeof WebAssembly.instantiateStreaming === "undefined") {
      compileNoStream(fetchPromise, info, receiveInstance);
    }
    else {
      let compileStart = performance.now();
      WebAssembly.instantiateStreaming(fetchPromise, info)
        .then((obj) => {
          Module["compileTime"] = performance.now() - compileStart;
          receiveInstance(obj.instance, obj.module);
        })
        .catch((reas) => {
          Module["printWarn"]("Failed to compile streaming: " + reas);
          Module["printWarn"]("Falling back to compile");
          compileNoStream(fetchPromise, info, receiveInstance);
        });
    }
  } else {
    let compileStart = performance.now();
    const data = new Uint8Array(readbuffer(wasmBinaryFile));
    let mod = new WebAssembly.Module(data);
    let inst = new WebAssembly.Instance(mod, info);
    Module["compileTime"] = performance.now() - compileStart;
    receiveInstance(inst, mod);
  }
};
