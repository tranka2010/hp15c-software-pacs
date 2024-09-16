% Copyright (C) 2024 Pepin Torres, P.E.
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <https://www.gnu.org/licenses/>.

% -*- texinfo -*-
% @deftypefn {} {@var{retval} =} sanity_check (@var{input1}, @var{input2})
%
% @seealso{}
% @end deftypefn

% Author: Pepin Torres, P.E. <pepin.torres@gmail.com>
% Created: 2024-08-07

function [C,Hx,Hsweep] = sanity_check(Fs,f0,BW,A,fx)

  % A = 10^(dBgain / 40)
  % BW is filter bandwidth in multiples of an octave
  % f0 is filter center frequemcy
  % Fs is sampling frequency
  % fx is frequency at which to evaluate the transfer function of the filters H(wx)

  clc
  w0 = 2*pi*f0/Fs;
  Q = 1/(2*sinh((log(2)/2)*BW*w0/sin(w0)));
  alpha = sin(w0)/(2*Q);

  % Individual biquads
  LPF = [ (1-cos(w0))/2    1-cos(w0)  (1-cos(w0))/2 1+alpha -2*cos(w0) 1-alpha];
  HPF = [ (1+cos(w0))/2  -(1+cos(w0)) (1+cos(w0))/2 1+alpha -2*cos(w0) 1-alpha];
  BPFq = [Q*alpha 0 -Q*alpha LPF(4:6)];
  BPF0 = [  alpha 0   -alpha LPF(4:6)];
  NOTCH = [1 -2*cos(w0) 1 LPF(4:6)];
  APF = [1-alpha -2*cos(w0) 1+alpha 1+alpha -2*cos(w0) 1-alpha];
  PEAK = [1+alpha*A  -2*cos(w0) 1-alpha*A 1+alpha/A  -2*cos(w0) 1-alpha/A];
  LOWSHELF = [A*((A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha) ...
            2*A*((A-1)-(A+1)*cos(w0)) ...
              A*((A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha) ...
                 (A+1)+(A-1)*cos(w0)+2*sqrt(A)*alpha ...
             -2*((A-1)+(A+1)*cos(w0)) ...
                 (A+1)+(A-1)*cos(w0)-2*sqrt(A)*alpha];

  HIGHSHELF = [A*((A+1)+(A-1)*cos(w0)+2*sqrt(A)*alpha) ...
            -2*A*((A-1)+(A+1)*cos(w0)) ...
              A*((A+1)+(A-1)*cos(w0)-2*sqrt(A)*alpha) ...
                 (A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha ...
              2*((A-1)-(A+1)*cos(w0)) ...
                 (A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha];

  % Matrix of biquads
  C = [LPF; HPF; BPFq; BPF0; NOTCH; APF; PEAK; LOWSHELF; HIGHSHELF]

  % Print biquads to screen to aid in sanity check / validation of code

  fprintf('Audio EQ Cookbook\nBiquads for LPF HPF BPFq BPF0 NOTCH APF PEAK LOWSHELF HIGHSHELF (per row)\n');
  fprintf('where f0 = %.4f, Fs = %.4f, A = %.4f, BW = %.4f, fx = %.4f\n\n',f0,Fs,A,BW,fx)
  fprintf('       b0  ,     b1  ,     b2  ,     a0  ,      a1  ,     a2\n')
  %        0.004278, 0.008555, 0.004278, 1.022798, -1.982890, 0.977202,
  for ii = 1:size(C,1)
      fprintf('%d) ',ii);
      for jj = 1:size(C,2)

          fprintf('%.6f, ',C(ii,jj));
      endfor
      fprintf('\n');
  endfor
  fprintf('\n');

  % Print z = exp(jw0) = exp(j2pifx/Fs) as calculated in HP15c
  zx = exp(j*2*pi*fx/Fs);
  fprintf("Powers of z for frequency fx = %.4f\n",fx)
  fprintf("z^0 = (%.8f,%.8f)\n",real(zx^0),imag(zx^0));
  fprintf("z^-1 = (%.8f,%.8f)\n",real(zx^-1),imag(zx^-1));
  fprintf("z^-2 = (%.8f,%.8f)\n\n",real(zx^-2),imag(zx^-2));

  % Show sum(bn * z^-n) and sum(an * z^-n) for each filter as calculated in HP15c
  fprintf('Intermediate complex vectors.\nE = sum(bn * zx^-n)\t\tD = sum(an * zx^-n)\n')
  for kk=1:size(C,1)
    ED(kk,:) = [ C(kk,1)*zx^0 + C(kk,2)*zx^-1 + C(kk,3)*zx^-2 C(kk,4)*zx^0 + C(kk,5)*zx^-1 + C(kk,6)*zx^-2];
    fprintf("E(%d) %.6f + %.6fj\tD(%d) %.6f + %.6fj\n",kk,real(ED(kk,1)),imag(ED(kk,1)),kk,real(ED(kk,2)),imag(ED(kk,2)));
  endfor
  fprintf('\n');

  % Show H(wx) as calculated in HP15c
 fprintf('Frequency response evaluated at fx = %.4f [ H(x) = E(wx)/D(wx) ]\n',fx)
  for kk=1:size(C,1)
    Hx(kk) = (C(kk,1)*zx^0 + C(kk,2)*zx^-1 + C(kk,3)*zx^-2) / (C(kk,4)*zx^0 + C(kk,5)*zx^-1 + C(kk,6)*zx^-2);
    fprintf("H(2*pi*%dHz) = %.8f + %.8fj\n",fx,real(Hx(kk)),imag(Hx(kk)));
  endfor

  for jj = 0:1:Fs/2
    zx = exp(j*2*pi*jj/Fs);
    for kk = 1:size(C,1)
      Hsweep(kk,jj+1) = (C(kk,1)*zx^0 + C(kk,2)*zx^-1 + C(kk,3)*zx^-2) / (C(kk,4)*zx^0 + C(kk,5)*zx^-1 + C(kk,6)*zx^-2);
    endfor
  endfor

  figure(1)
  clf
  semilogx(20*log10(abs(Hsweep(4,:))));
  hold on
  semilogx(fx,20*log10(abs(Hx(4))),'x');
  semilogx(f0,20*log10(abs(1)),'o');
  title('H(w) for Bandpass Filter with f0 = 1KHz and fx = 840Hz')
  legend('H(f)','H(fx)','H(f0)')
  xlabel('frequency [Hz]')
  ylabel('Amplitude [dB]')

