N = 32;
n = [0:N-1];
alpha = 0.54;
x = alpha - (1-alpha)*cos(2*pi*n/N)
x_coherent = sum(x)/N
x_enbw = N*sum(abs(x).^2)/(sum(x).^2)
x_pl = 10*log10(1/x_enbw)
x_scallop = 20*log10(abs(sum(x.*exp(-j*pi*n/N)))/sum(x))
x_worst = x_pl + x_scallop
plot(x)
