% Copyright (C) 2024 pepin
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

% Author: pepin <pepin@PEPIN-HP-LAPTOP>
% Created: 2024-08-07

function [C,Q] = sanity_check(Fs,f0,BW,A)

  w0 = 2*pi*f0/Fs;
  Q = 1/(2*sinh((log(2)/2)*BW*w0/sin(w0)));
  alpha = sin(w0)/(2*Q);

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

   C = [LPF; HPF; BPFq; BPF0; NOTCH; APF; PEAK; LOWSHELF; HIGHSHELF; [Fs f0 BW A Q alpha]];

   for ii = 1:size(C,1)
       for jj = 1:size(C,2)
           fprintf('%.6f, ',C(ii,jj));
       end
       fprintf('\n');
   end
end