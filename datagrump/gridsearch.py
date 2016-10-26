#!/usr/bin/env python3

import subprocess
import os

SPACE = {
    'RTT_EWMA_FACTOR': ['0.2', '0.5', '1.0'],
    'TARGET_DELAY': ['70.0', '90.0'],
    'K_P': ['1e0', '5e-1', '1e-1', '5e-2', '1e-2', '5e-3', '1e-3'],
    'K_I': ['0.0'],
    'K_D': ['1e-1', '1e-2', '1e-3', '5e-4', '1e-4', '5e-5', '1e-5'],
}

def grid(space):
    res = [{}]
    for key, values in space.items():
        res = [dict(r, **{key: v}) for r in res for v in values]
    return res

def write_parameters(params):
    template = '''
/* AUTO GENERATED FILE, DO NOT MODIFY */
#ifndef PARAMETERS_HH
#define PARAMETERS_HH

%s

#endif /* PARAMETERS_HH */
'''.lstrip()
    defines = '\n'.join('#define %s (%s)' % i for i in params.items())
    with open('parameters.hh', 'w') as f:
        f.write(template % defines)

def get_scores():
    ret = subprocess.call(['make'])
    assert ret == 0
    proc = subprocess.Popen(['./run-contest', 'dummy'], stderr=subprocess.PIPE)
    _, data = proc.communicate()
    data = data.decode('utf8')
    throughput = None
    delay = None
    for line in data.split('\n'):
        if line.startswith('Average throughput:'):
            assert line.split()[3] == 'Mbits/s'
            throughput = float(line.split()[2])
        if line.startswith('95th percentile signal delay:'):
            assert line.split()[5] == 'ms'
            delay = int(line.split()[4])
    assert throughput is not None
    assert delay is not None
    return (throughput, delay, (throughput * 1000 / delay))

def main():
    scores = [] # tuples of (params, throughput, delay, score)
    for params in grid(SPACE):
        write_parameters(params)
        print('Running with parameters: %s' % params)
        throughput, delay, score = get_scores()
        print('Result: throughput: %.2f, delay: %d, score: %.2f' % (throughput, delay, score))
        scores.append((params, throughput, delay, score))
    print('\n\n')
    scores.sort(key=lambda i: i[3])
    print('params, throughput, delay, score')
    print('\n'.join(str(i) for i in scores))

if __name__ == '__main__':
    main()
