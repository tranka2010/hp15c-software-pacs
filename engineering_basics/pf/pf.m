%% Partial Fraction Decomposition
clear variables
clc

z = 1;
r = 2;

A = [0   0  0  2 -1; ...
    -2 -2 -2 -3 -3];
[rows,cols] = size(A);
B = zeros(1,cols);
C = eye(cols);
D = zeros(1,cols);
E = zeros(rows,cols);

isRootUnique = true;
reduceOrder = 1;
for m = 1 : cols
    B = B*0;
    if m > 1
      if A(r,m) == A(r,m-1)
        isRootUnique = false;
      else
        isRootUnique = true;
      end
    end
    if isRootUnique == false
        reduceOrder=reduceOrder+1;
        B(reduceOrder) = 1;
        lastRepeatedRoot = m;
    else
        B(1) = 1;
        reduceOrder = 1;
        firstRepeatedRoot = m;
        lastRepeatedRoot = firstRepeatedRoot;
    end
    for n = 1 : cols
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

