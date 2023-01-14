
# Table of Contents

1.  [Gaussian Noise generator](#org82e3c04)
        1.  [Box and Muller method](#org81d6eff)
        2.  [Marsaglia method](#org3afaa9c)
    1.  [Test](#orgaf6cc59)
        1.  [Box and Muller method](#org8fdbd14)
        2.  [Testing source blocks](#org3740e1b)



<a id="org82e3c04"></a>

# Gaussian Noise generator

Two simple ways of generating samples from a Gaussian distribution:


<a id="org81d6eff"></a>

### Box and Muller method


<a id="org3afaa9c"></a>

### Marsaglia method


<a id="orgaf6cc59"></a>

## Test


<a id="org8fdbd14"></a>

### Box and Muller method

    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    import numpy as np

As a test, we&rsquo;ll generate samples from a normal distribution using numpy&rsquo;s blackbox:

    mu = 0.0
    sigma = 1.0
    number_of_samples = 200
    normal_samples = np.random.default_rng().normal(mu, sigma, number_of_samples)

Sanity checks:

    mean_near_mu = abs( mu - np.mean(normal_samples) )
    std_near_sigma = abs(sigma - np.std(normal_samples, ddof=1))
    
    assert(mean_near_mu < 0.2)
    assert(std_near_sigma < 0.2)

Plot the above

    fig=plt.figure(figsize=(4,2))
    count, bins, ignored = plt.hist(normal_samples, 30, ec='orange', fc='steelblue', density=True)
    plt.plot(bins, 1/(sigma * np.sqrt(2 * np.pi)) *
                   np.exp( - (bins - mu)**2 / (2 * sigma**2) ),
             linewidth=2, color='violet', alpha=0.5)
    fig.tight_layout()
    fname="images/numpyExpectationsBarChart.png"
    plt.savefig(fname)
    fname

Compare with data generated from our own gaussian sampling method:

    import math
    import random
    
    def getGaussianPair():
        u = random.random()
        v = random.random()
        while u == 0:
            u = random.random()
        sqrt_minustwo_logu = math.sqrt(-2 * math.log(u))
        two_pi_v = 2 * math.pi * v
        z_0 = sqrt_minustwo_logu * math.cos(two_pi_v)
        z_1 = sqrt_minustwo_logu * math.sin(two_pi_v)
        return [z_0, z_1]
    
    def sample():
        if (self.has_spare):
          self.sample.append(self.spare)
          self.has_spare = False
        else:
          self.spare, result = getGaussianPair()
          self.has_spare = True
          self.sample.append(result)
    
    def getSample(sample_size):
        result = []
        for _ in range(sample_size):
            result.append(getGaussianPair()[0])
            print(result)

    sample = getSample(number_of_samples)
    
    fig=plt.figure(figsize=(4,2))
    count, bins, ignored = plt.hist(sample, 30, ec='orange', fc='steelblue', density=True, histtype='barstacked')
    plt.plot(bins, 1/(sigma * np.sqrt(2 * np.pi)) *
                   np.exp( - (bins - mu)**2 / (2 * sigma**2) ),
             linewidth=2, color='violet', alpha=0.5)
    fig.tight_layout()
    fname="images/expectationsBarChart.png"
    plt.savefig(fname)
    fname


<a id="org3740e1b"></a>

### Testing source blocks

