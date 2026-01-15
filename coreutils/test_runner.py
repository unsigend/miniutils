#!/usr/bin/env python3
"""
mini-coreutils Test Framework
A simple test runner that compares custom implementations with system utilities.
Author: Yixiang Qiu
"""

import sys
import subprocess
import difflib
from pathlib import Path

# ANSI color codes
class Colors:
    RESET = '\033[0m'
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'
    BLUE = '\033[34m'
    MAGENTA = '\033[35m'
    CYAN = '\033[36m'
    BOLD = '\033[1m'

def colorize(text, color):
    """Apply color to text."""
    return f"{color}{text}{Colors.RESET}"

def get_test_dir():
    """Get the test directory path."""
    script_dir = Path(__file__).parent.absolute()
    return script_dir / "test"

def list_available_tests():
    """List all available test procedures."""
    test_dir = get_test_dir()
    if not test_dir.exists():
        return []
    
    tests = []
    for item in test_dir.iterdir():
        if item.is_dir():
            tests.append(item.name)
    return sorted(tests)

def read_test_commands(test_name):
    """Read all test commands from the test directory."""
    test_dir = get_test_dir() / test_name
    command_file = test_dir / "command"
    
    if not command_file.exists():
        print(colorize(f"Error: command file not found in {test_dir}", Colors.RED))
        return None
    
    try:
        with open(command_file, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except (IOError, OSError) as e:
        print(colorize(f"Error: Cannot read command file: {e}", Colors.RED))
        return None
    
    # Extract all non-empty, non-comment lines as separate test cases
    commands = []
    for line in lines:
        line = line.strip()
        # Skip empty lines and comments
        if line and not line.startswith('#'):
            commands.append(line)
    
    if not commands:
        print(colorize(f"Error: command file is empty or contains only comments", Colors.RED))
        return None
    
    return commands

def run_command(cmd, shell=False, cwd=None):
    """Run a command and return (stdout, stderr, returncode)."""
    try:
        result = subprocess.run(
            cmd,
            shell=shell,
            capture_output=True,
            text=True,
            timeout=30,
            cwd=cwd
        )
        return result.stdout, result.stderr, result.returncode
    except subprocess.TimeoutExpired:
        return "", "Command timed out", 1
    except Exception as e:
        return "", str(e), 1

def normalize_output(text):
    """Normalize output for comparison (strip trailing whitespace)."""
    lines = text.splitlines()
    # Remove trailing whitespace from each line
    normalized = [line.rstrip() for line in lines]
    # Remove trailing empty lines
    while normalized and not normalized[-1]:
        normalized.pop()
    return '\n'.join(normalized) + ('\n' if normalized else '')

def highlight_differences_in_line(old_line, new_line):
    """Highlight only the differing parts between two lines."""
    matcher = difflib.SequenceMatcher(None, old_line, new_line)
    highlighted_old = []
    highlighted_new = []
    
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == 'equal':
            highlighted_old.append(old_line[i1:i2])
            highlighted_new.append(new_line[j1:j2])
        elif tag == 'delete':
            highlighted_old.append(colorize(old_line[i1:i2], Colors.RED))
        elif tag == 'insert':
            highlighted_new.append(colorize(new_line[j1:j2], Colors.GREEN))
        elif tag == 'replace':
            highlighted_old.append(colorize(old_line[i1:i2], Colors.RED))
            highlighted_new.append(colorize(new_line[j1:j2], Colors.GREEN))
    
    return ''.join(highlighted_old), ''.join(highlighted_new)

def compare_outputs(expected, actual, expected_name, actual_name):
    """Compare two outputs and highlight differences."""
    expected_norm = normalize_output(expected)
    actual_norm = normalize_output(actual)
    
    if expected_norm == actual_norm:
        return True, None
    
    # Generate unified diff
    expected_lines = expected_norm.splitlines(keepends=True)
    actual_lines = actual_norm.splitlines(keepends=True)
    
    diff = list(difflib.unified_diff(
        expected_lines,
        actual_lines,
        fromfile=expected_name,
        tofile=actual_name,
        lineterm=''
    ))
    
    return False, diff

def print_diff(diff_lines):
    """Print diff with color highlighting - only highlight differences within lines."""
    i = 0
    while i < len(diff_lines):
        line = diff_lines[i]
        if line.startswith('---'):
            print(colorize(line, Colors.CYAN), end='')
        elif line.startswith('+++'):
            print(colorize(line, Colors.CYAN), end='')
        elif line.startswith('@@'):
            print(colorize(line, Colors.YELLOW), end='\n')
        elif line.startswith('-') and i + 1 < len(diff_lines) and diff_lines[i + 1].startswith('+'):
            old_line = line[1:]
            new_line = diff_lines[i + 1][1:]
            old_highlighted, new_highlighted = highlight_differences_in_line(old_line, new_line)
            print(colorize('-', Colors.RED) + old_highlighted, end='')
            print(colorize('+', Colors.GREEN) + new_highlighted, end='')
            i += 1
        elif line.startswith('-'):
            # Unpaired deletion
            print(colorize('-', Colors.RED) + line[1:], end='')
        elif line.startswith('+'):
            # Unpaired insertion
            print(colorize('+', Colors.GREEN) + line[1:], end='')
        else:
            # Context lines - print without color
            print(line, end='')
        i += 1

def run_single_test_case(test_name, test_cmd, test_num, total_tests):
    """Run a single test case. Only outputs on failure."""
    # Parse command: command file contains arguments to pass to the program
    # The binary path is always build/bin/<test_name>
    test_args = test_cmd.split()
    
    # Binary path relative to script location: ../build/bin/<test_name>
    script_dir = Path(__file__).parent.absolute()
    root_dir = script_dir.parent
    binary_path_relative = Path("..") / "build" / "bin" / test_name
    binary_path_absolute = (script_dir / binary_path_relative).resolve()
    
    if not binary_path_absolute.exists():
        print(colorize(f"\n[Test {test_num}/{total_tests}]", Colors.BOLD))
        print(colorize(f"Error: Binary not found: {binary_path_absolute}", Colors.RED))
        return False
    
    # System command: test_name + arguments from command file
    # Custom command: relative path to binary + arguments from command file
    # Commands will run from project root, so paths in test_args work as-is
    system_cmd = [test_name] + test_args
    custom_cmd_display = [str(binary_path_relative)] + test_args
    custom_cmd_execute = [str(binary_path_absolute)] + test_args
    
    # Run commands from project root directory
    sys_stdout, sys_stderr, sys_rc = run_command(system_cmd, cwd=str(root_dir))
    custom_stdout, custom_stderr, custom_rc = run_command(custom_cmd_execute, cwd=str(root_dir))
    
    # Compare stdout
    stdout_match, stdout_diff = compare_outputs(sys_stdout, custom_stdout, "system", "custom")
    
    # Compare stderr
    stderr_match, stderr_diff = compare_outputs(sys_stderr, custom_stderr, "system (stderr)", "custom (stderr)")
    
    if stdout_match and stderr_match:
        # Success - no output
        return True
    else:
        # Failure - show output
        # print(colorize(f"\n[Test {test_num}/{total_tests}]", Colors.BOLD))
        # print(colorize(f"{'='*60}", Colors.BOLD))
        print(colorize("FAILED", Colors.RED))
        print(f"System command: {' '.join(system_cmd)}")
        print(f"Custom command: {' '.join(custom_cmd_display)}")
        
        if not stdout_match:
            print(colorize("\n--- STDOUT DIFF ---", Colors.BOLD))
            print_diff(stdout_diff)
        
        if not stderr_match:
            print(colorize("\n--- STDERR DIFF ---", Colors.BOLD))
            print_diff(stderr_diff)
        
        return False

def run_test(test_name):
    """Run all test cases for a test procedure."""
    print(colorize(f"Running test procedure: {test_name}", Colors.BOLD))
    
    # Read all test commands
    test_commands = read_test_commands(test_name)
    if not test_commands:
        return False
    
    total_tests = len(test_commands)
    passed_tests = 0
    
    # Run each test case
    for i, test_cmd in enumerate(test_commands, 1):
        if run_single_test_case(test_name, test_cmd, i, total_tests):
            passed_tests += 1
    
    # Display statistics
    print(f"\nTotal: {passed_tests}/{total_tests}")
    
    return passed_tests == total_tests

def main():
    """Main entry point."""
    # List available tests
    available_tests = list_available_tests()
    
    if not available_tests:
        print(colorize("Error: No test procedures found in test directory", Colors.RED))
        sys.exit(1)
    
    # Prompt for test name
    print(colorize("\nAvailable test procedures:", Colors.BOLD))
    for i, test in enumerate(available_tests, 1):
        print(f"  {i}. {test}")
    
    print()
    user_input = input("Enter test procedure name or number: ").strip()
    
    if not user_input:
        print(colorize("Error: No test name or number provided", Colors.RED))
        sys.exit(1)
    
    # Check if input is a number
    test_name = None
    try:
        test_num = int(user_input)
        if 1 <= test_num <= len(available_tests):
            test_name = available_tests[test_num - 1]
        else:
            print(colorize(f"Error: Invalid test number {test_num}", Colors.RED))
            print(colorize(f"Please enter a number between 1 and {len(available_tests)}", Colors.YELLOW))
            sys.exit(1)
    except ValueError:
        # Not a number, treat as test name
        test_name = user_input
    
    if test_name not in available_tests:
        print(colorize(f"Error: Test procedure '{test_name}' not found", Colors.RED))
        print(colorize(f"Available tests: {', '.join(available_tests)}", Colors.YELLOW))
        sys.exit(1)
    
    # Run the test
    success = run_test(test_name)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
