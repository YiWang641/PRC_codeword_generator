#!/usr/bin/env python3
'''
Estimate the minimal k for PRC watermark detection:

    k >= 8 * ln(2*n/sec) / (1 - 2*p)^4

Usage:
    python k_estimate.py --n 1e6 --p 0.1 --sec 2**-80
'''

import math
import argparse

def parse_float_or_expr(s: str) -> float:
    '''Parse a string as float, or evaluate it as a Python expression.'''
    try:
        return float(s)
    except ValueError:
        # Evaluate simple numeric expressions like '2**-80'
        return float(eval(s, {"__builtins__": {}}))

def estimate_k(n: float, p: float, sec: float) -> int:
    '''Compute the minimal integer k satisfying the inequality.'''
    numerator = 8 * math.log(2 * n / sec)
    denom = (1 - 2 * p) ** 4
    return math.ceil(numerator / denom)

def main():
    parser = argparse.ArgumentParser(
        description="Estimate minimal k for PRC watermark detection"
    )
    parser.add_argument(
        "--n", type=float, default=1e6,
        help="Number of parity-check sets (default: 1e6)"
    )
    parser.add_argument(
        "--p", type=float, default=0.1,
        help="Noise rate (default: 0.1)"
    )
    parser.add_argument(
        "--sec", type=parse_float_or_expr, default=2**-80,
        help="Security parameter (default: 2**-80). Can be float or expression."
    )
    args = parser.parse_args()

    k = estimate_k(args.n, args.p, args.sec)
    print(f"Estimated minimal k: {k}")
    print("Parameters used:")
    print(f"  n   = {args.n}")
    print(f"  p   = {args.p}")
    print(f"  sec = {args.sec}")
    print("Formula: k >= 8 * ln(2*n/sec) / (1 - 2*p)^4")

if __name__ == "__main__":
    main()
