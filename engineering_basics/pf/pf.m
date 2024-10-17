%% Partial Fraction Decomposition
clear variables
clc

z = 1;
r = 2;

A = [0   0  0  2 -1; ...
    -2 -2 -2 -3 -3];
##A = [0   0  0  2 -1; ...
##    -1 -2 -3 -4 -5];

[rows,cols] = size(A);
B = zeros(1,cols);
C = eye(cols);
D = zeros(1,cols);
E = zeros(rows,cols);

isRootUnique = true; % R.1
reduceOrderBy = 1; % R.2
for m = 1 : cols % R.9
    B = B*0;
    if m > 1
      if A(r,m) == A(r,m-1)
        isRootUnique = false;
      else
        isRootUnique = true;
      end
    end
    if isRootUnique == false
        reduceOrderBy=reduceOrderBy+1; % R.2
        B(reduceOrderBy) = 1;
        lastRepeatedRoot = m; % R.4
    else
        B(1) = 1;
        reduceOrderBy = 1; % R.2
        firstRepeatedRoot = m; % R.3
        lastRepeatedRoot = m; % R.4
    end
    for n = 1 : cols % R.8
        xp = A(r,n);
        if (n == m || (n >= firstRepeatedRoot && n <= lastRepeatedRoot))
            xp = 0;
        end
        for k = 1 : cols-1
            C(k,k+1) = xp;
        end
        D = (B*C);
        B = D;
    end
        fprintf('%4d ',B)
    for k = 1:n
        E(k,m) = B(k);
    end
    fprintf('\n\n')
end

B = inv(E)*A.';
B = B';
B = B(1,:);

fprintf('==========================\n')

fprintf('%4d ',B)
fprintf('\n\n')


%% Expected results


##A = [0   0  0  2 -1; ...
##    -2 -2 -2 -3 -3];


##   1  -10   37  -60   36
##
##   0    1   -8   21  -18
##
##   0    0    1   -6    9
##
##   1   -9   30  -44   24
##
##   0    1   -6   12   -8
##
##==========================
##  13    8    3  -13    5


##A = [0   0  0  2 -1; ...
##    -1 -2 -3 -4 -5];

##   1  -14   71 -154  120
##
##   1  -13   59 -107   60
##
##   1  -12   49  -78   40
##
##   1  -11   41  -61   30
##
##   1  -10   35  -50   24
##
##==========================
##0.0416667 -0.5 1.25 -1.16667 0.375
##

