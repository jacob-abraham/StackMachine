


let StackMachine = null;

function onload(verbose) {
  createModule()
    .then(mod => {
      StackMachine = mod;
      if(verbose) {
        console.log("Successful compile in ", mod["compileTime"], "ms");
      }
    })
    .catch(err => console.error(err));
}


function parse(input) {
  //if no Module yet, wait, compilation should be very fast
  if(StackMachine == null) {
    setTimeout(() => {
      parse(input);
    }, 500);
  }
  else {
    //let lexer = StackMachine.build_Lexer(input);
    /*let res = lexer.getToken().lexeme;
    console.log("lexeme: [", res ,"]");
    console.log("lexeme: [", res.charCodeAt(0)," ", res.charCodeAt(1), " ",res.charCodeAt(2),"]");
    */
    //lexer.delete();

    let parser;
    try {
      StackMachine.test(input);
      parser = new StackMachine.Parser(input);
      parser.parse();
      console.log("hello")
      let mem = pasrser.getMemory();
      console.log(mem.getMemString())
    }
    catch(e) {
      if(typeof e === "number")
        console.error(StackMachine.getExceptionMessage(e))
      else
        console.error(e)
    }
    finally {
      if(parser)
        parser.delete();
    }
  }
}

onload(true);