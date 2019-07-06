import matplotlib.pyplot as plt
import numpy as np


## ------------------------ Question 2 -----------------------------------------------#

## Definition of functions and derivatives of functions
# This is used later in calculation of the new position during the line deach


# objective functions and derivative
def obj(X):
    return (X[0] + 1) * (X[0] + 1) + 0.5 * X[1]*X[1]

def d_obj(X):
    return np.array([2*(X[0] + 1), X[1]])


# constraints and their derivatives.
def g1(X):
    return -1 * (X[0] - 3)

def d_g1(X):
    return np.array([c*2*(X[0] - 3), 0])

def g2 (X):
    return X[1] 
def d_g2(X):
    return np.array([0 , c*2*X[1]])

def g3 (X):
    return (1/8)*X[0]*X[0]*X[0] - X[1]
def d_g3 (X):
    return np.array([3*c*X[0]**2*(X[0]**3 - 8*X[1])/32, c*(-X[0]**3 + 8* X[1])/4], dtype='float64')


# list of constraints and derivatives of constraints 
g = [g1,g2,g3]
d = [d_g1,d_g2,d_g3]

obj, constraints, derivatives = obj,g, d

def bracket(value):
    return min(0,value) ## check positive or negative

# Function to return the solution fo the augmented penalty function for any position  based on which 
# constraints are violated
def penalty(X): ##calculate and return the penalty
    global obj
    global constraints
    global c
    objective = obj(X)
    extra = 0
    for constraint in constraints:
        penalty = bracket(constraint(X))**2         
        extra += penalty
    # returns the total penalty and penalty due to constraints
    return objective + c*extra

# function to return the solution of the gradients based on the augmented penalty function and which 
#constraints are violated
def grad(X):
    global constraints
    global derivatives
    total = d_obj(X)

    for i in range(len(constraints)):
        if constraints[i](X)<=0:
            total += derivatives[i](X)

    return total


# simple implementation of a search technique to get new positio based on derivates and previous position
def get_pos(X):
    position = X
    new_pos = position - growth*grad([position[0],position[1]])
    return new_pos


# main program

max_iter=1000000  # max iterations to run for
c=400000           # penalty for violation 
growth =0.000001    # growth term
start_pos=(1,0.1)      #starting position


to_plot = []

position = start_pos

while max_iter:
    position = get_pos(position)
    to_plot.append(obj(position)) 
    max_iter -= 1

# to plot graph at the end of solution

plt.figure()
plt.title('Convergence')
plt.ylabel(r'$(x +1)^2 + 1/2*y^2$')
plt.xlabel('Iterations')
plt.plot(range(0,len(to_plot)),to_plot)
plt.show()
plt.savefig('conv_2')