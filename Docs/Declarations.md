## Declarations

A1 programming language supports:

- [variable declaration](#variable-declaration)
- [function declaration](#function-declaration)
- [class declaration](#class-declaration)
- [smart contract declaration](#smart-contract-declaration)

If you find this useful, please check out our [Examples](../Examples) for more interesting content.

### Variable declaration

Variables are declared by using the `let` keyword. Since A1's focus is simplicity, type declarations are optional but recommended, since they improve readability of the code significantly. Therefore, you can declare a variable simply like:

```
let foo = 0.12
```

and it's type will automatically be deduced to float, or you can declare the type yourself like:

```
let foo: num = 0.12
```

Both works and both is correct!

### Function declaration

Functions are declared pretty much the same as functions in Python programming language. However, unlike Python, A1 enforces specifying parameter types and return types.

If the function does not return a value, return type should be omitted.

Here is a short example:

```
def sum(x: num, y: num) -> num:
    let c = x + y
    return x
```

### Class declaration

Classes in A1 are the same as classes in Python. One tiny difference is the data member declaration with `let` keyword that we have already spoken about previously.

Here is a short example of a class in A1 programming language:

```
class Proposal:
    let name: str
    let count: num
```

### Smart contract declaration

Smart contracts in A1 are conceptually similar to classes in Python except that they support managing the blockchain storage, various blockchain-related operations, etc.

Here is a short example:

```
# ... the rest of the code defining Voter and Proposal classes

contract Ballot:
    let voters: map[address, Voter]
    let proposals: array[Proposal]

    def vote(proposalIdx: num):
        voter = voters[msg.sender]
        voter.hasVoted = True
        voter.vote = proposalIdx

        proposals[proposalIdx].count += voter.weight
```
