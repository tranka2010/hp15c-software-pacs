N = 32;
n = [0:N-1]-N/2;
alpha = 2;
x = cos(pi*n/N).^2

fig_merit_sim(x,N);
