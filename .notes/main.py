import math
import random

def sample_gaussian():
    u = random.random()
    v = random.random()
    while u == 0:
        u = random.random()
    sqrt_minustwo_logu = math.sqrt(-2 * math.log(u))
    two_pi_v = 2 * math.pi * v
    z_0 = sqrt_minustwo_logu * math.cos(two_pi_v)
    z_1 = sqrt_minustwo_logu * math.sin(two_pi_v)
    return [z_0, z_1]
