N = 32;
n = [0:N-1];
alpha = 0.54;
x = alpha - (1-alpha)*cos(2*pi*n/N);

fig_merit_sim(x,N);

