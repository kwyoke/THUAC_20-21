import numpy as np
from sklearn.datasets import load_svmlight_file
import matplotlib.pyplot as plt
from sklearn.model_selection import KFold

path_train = "a9a/a9a"
path_test = "a9a/a9a.t"

#####PARAMS TO DEFINE#######
default_split = False
k = 10
w_init = np.zeros((124,1)) + 0.01
MAX_ITER = 100
#L2s = [0, 0.1, 1, 10, 20, 35, 50, 75, 100]  # good for default-split
L2s = [0, 0.001, 0.01, 0.1, 1, 2, 5, 6, 7.5, 10] # good for kfold
MIN_DIFF = 0.001
np_dtype = np.float32

#########LOAD DATA##########
# load all data
X_train, y_train = load_svmlight_file(path_train, n_features=123, dtype=np_dtype)
X_test, y_test = load_svmlight_file(path_test, n_features=123, dtype=np_dtype)


# stack a dimension of ones to X to simplify computation
N_train = X_train.shape[0]
N_test = X_test.shape[0]
X_train = np.hstack((np.ones((N_train, 1)), X_train.toarray())).T
X_test = np.hstack((np.ones((N_test, 1)), X_test.toarray())).T

y_train = y_train.reshape((N_train, 1))
y_test = y_test.reshape((N_test, 1))

# X_train: (124, 32561), y_train: (32561, 1)
# X_test:  (124, 16281), y_test:(16281, 1)

# label: -1, +1 ==> 0, 1
y_train = np.where(y_train == -1, 0, 1)
y_test = np.where(y_test == -1, 0, 1)

X_all = np.hstack((X_train, X_test)).T
y_all = np.vstack((y_train, y_test))


#######FUNCTIONS############
def sigmoid(x):
    ans = np.where(x<0, np.exp(x)/ (1 + np.exp(x)), 1/(1 + np.exp(-x)))
    return ans

def predict(w, X):
    wX = (w.T @ X).T
    yhat = np.where(wX >=0, 1, 0)
    return yhat

def compute_acc(w, X, y):
    y_hat = predict(w,X)
    acc = np.mean(y_hat == y)
    return acc

def update_weights(w_old, X, y, l2):
    '''
    w_old: dx1
    X    : dxN
    y    : Nx1
    l2   : lambda L2 reg param, L(w) = log_likelihood - lambda*||w||^2
    '''
    
    d, N = X.shape
    
    wX = (w_old.T @ X).T # Nx1
    mu = sigmoid(wX) # Nx1
    R_flat = mu*(1-mu) # Nx1
    XRX = X @ (R_flat * X.T) # dxd
    
    H = -XRX - l2*np.eye(d)
    Hinv = np.linalg.pinv(H)
    
    w_new = w_old - Hinv @ (X@(y-mu) - l2*w_old)
    
    return w_new


def train_logreg_IRLS(X_train, y_train, X_test, y_test, L2, w_init, MAX_ITER, MIN_DIFF):
    w_old = w_init

    i = 0
    train_accs = []
    test_accs = []
    w_norms = []

    while (i<MAX_ITER):
        w_new = update_weights(w_old, X_train, y_train, L2)

        w_diff = np.linalg.norm(w_new-w_old)
        w_norm = np.linalg.norm(w_new)
        train_acc = compute_acc(w_new, X_train, y_train)
        test_acc = compute_acc(w_new, X_test, y_test)

        train_accs.append(train_acc)
        test_accs.append(test_acc)
        w_norms.append(w_norm)

        i+=1
        w_old = w_new.copy()

        if w_diff < MIN_DIFF:
            break

    print('training done', i, train_acc, test_acc, w_norm)
    return w_new, i,  w_norms, train_accs, test_accs


if (default_split):
    ########MAIN 1: Default train-test split###################
    all_train_accs = []
    all_test_accs = []
    all_w_norms = []
    all_its = []
    for L2 in L2s:
        i = 0
        
        w, i,  w_norms, train_accs, test_accs = train_logreg_IRLS(X_train, y_train, X_test, y_test, L2, w_init, MAX_ITER, MIN_DIFF)
        all_train_accs.append(train_accs[-1])
        all_test_accs.append(test_accs[-1])
        all_w_norms.append(w_norms[-1])
        all_its.append(i)

else:
    #################MAIN 2 -- K fold#####################
    kfold = KFold(n_splits = k, shuffle=True, random_state=1)

    all_train_accs = []
    all_test_accs = []
    all_w_norms = []
    all_its = []
    for L2 in L2s:
        i_mean = 0
        train_acc_mean = 0
        test_acc_mean = 0
        w_norm_mean = 0
        # enumerate splits
        for train_idx, test_idx in kfold.split(X_all):
            train_X = X_all[train_idx]
            test_X = X_all[test_idx]
            train_y = y_all[train_idx]
            test_y = y_all[test_idx]

            train_X = train_X.T
            test_X = test_X.T

            w, i,  w_norms, train_accs, test_accs = train_logreg_IRLS(train_X, train_y, test_X, test_y, L2, w_init, MAX_ITER, MIN_DIFF)
            i_mean += i
            train_acc_mean += train_accs[-1]
            test_acc_mean += test_accs[-1]
            w_norm_mean += w_norms[-1]
        i_mean /=k
        train_acc_mean /= k
        test_acc_mean /= k
        w_norm_mean /= k

        all_train_accs.append(train_acc_mean)
        all_test_accs.append(test_acc_mean)
        all_w_norms.append(w_norm_mean)
        all_its.append(i_mean)
        print('L2 = ', L2, ' num_it: ', i_mean, ' w_norm: ', w_norm_mean, ' train_acc: ', train_acc_mean, 'test_acc: ', test_acc_mean)

#############PLOT####################
plt.figure(figsize=(12,5))
plt.subplot(1,3,1)
plt.title("Prediction accuracies over different lambda")
plt.xlabel('Lambda')
plt.ylabel('Prediction accuracy')
plt.plot(L2s, all_train_accs, 'b-', label='train_acc')
plt.plot(L2s, all_test_accs, 'r-', label='test_acc')
plt.legend()
plt.grid(True)
plt.subplot(1,3,2)
plt.title('L2 weight norms over different lambda')
plt.xlabel('Lambda')
plt.ylabel('L2 weight norms')
plt.plot(L2s, all_its, 'g-')
plt.grid(True)
plt.subplot(1,3,3)
plt.title('Num iterations to converge for different lambda')
plt.xlabel('Lambda')
plt.ylabel('Num iterations to convergence')
plt.plot(L2s, all_its)
plt.grid(True)
plt.tight_layout()
plt.show()
