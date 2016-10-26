#!/usr/bin/env python3

import subprocess
import os
import sys

SPACE = {
    'RTT_EWMA_FACTOR': ['0.2'],
    'TARGET_DELAY': ['60.0', '70.0', '80.0'],
    'K_P': ['1e-2', '1e-3'],
    'K_I': ['0.0'],
    'K_D': ['1e-2', '1e-3'],
    'MD_FACTOR': ['1.0', '1.1', '2.0'],
    'TIMEOUT': ['5000', '10000'],
    'LATE_FACTOR': ['1.0']
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
    ret = subprocess.call('make >/dev/null 2>&1', shell=True)
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
        sys.stdout.flush()
        throughput, delay, score = get_scores()
        print('Result: throughput: %.2f, delay: %d, score: %.2f' % (throughput, delay, score))
        sys.stdout.flush()
        scores.append((params, throughput, delay, score))
    print('\n\n')
    scores.sort(key=lambda i: i[3])
    print('params, throughput, delay, score')
    print('\n'.join(str(i) for i in scores))
    sys.stdout.flush()

if __name__ == '__main__':
    main()
