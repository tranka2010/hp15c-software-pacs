function fig_merit_sim(x,N)
  n = 0:N-1;
  x_coherent = sum(x)/N
  x_enbw = N*sum(abs(x).^2)/(sum(x).^2)
  x_pl = 10*log10(1/x_enbw)
  x_scallop = 20*log10(abs(sum(x.*exp(-j*pi*n/N)))/sum(x))
  x_worst = x_pl + x_scallop
  X = 20*log10(abs(fft(x)/N)).'

  figure(1)
  subplot(2,1,1)
  plot(x)
  subplot(2,1,2)
  plot(X)
  endfunction
