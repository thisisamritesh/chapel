config const iters = 200;
config const trials = 500;
config const n = 20;

var D: domain(real);

for i in 1..trials {
  coforall t in 1..n {
    const b = if t & 1 then -1 else 1;
    var r = 1..iters by b;
    for j in r do
      if !D.member((t+j):real) then
        D += (t+j);
  }
  for j in 2..n+iters do assert(D.member(j));
  D.clear();
}
