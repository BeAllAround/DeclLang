


;(function() {
  const LOAD = 1000000;

  {

    const h01 = {}

    // Insertion
    for(let i = 0; i < LOAD; i++) {
      h01[i] = i;
    }


    // let start = process.hrtime.bigint(); // Date.now()
    // let start = process.cpuUsage()
    let start = performance.now()

    for(let i = 0; i < LOAD; i++) {
      let value = h01[i];
    }

    // console.debug(process.hrtime.bigint() - start)

    // console.debug(process.cpuUsage(start))
    //
    console.debug((performance.now() - start))


  }
})();
