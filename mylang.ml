// types
nothing (null/void type)
bit 0 | 1 (alias: state     true | false)
    - byte            10011001
        - unsigned8 | signed8 | .16,.32,.64,.128
            - number (signed64)  -12312.123 | -134 | 28 | 293084.3490349
memory address

// bitwise operators
& |

binary          0010101011010101001010101011
hexadecimal     af2e31df
octal           2312346
integer         123912039

// object definitions
User (
    key: type,
)

// variable expression
// identifier type: value
userCount u16: 1201203
firstUser User: (
    name: string
    age: number
)
permissions User[]: [(), (), ()]
variableArray []: [123, true, ()]

// functions
getUserCount number or string: (account: string: "user") {
    if account = "banned" {
        exit "error" // early exit of function requires exit statement
    }

    5 // last statement is returned automatically
}

getFirstAndLast user, user: {
    // code block

    firstUser, lastUser // multi-return
}

// no async await syntax so we always know when things are async or sync
thisIsAlwaysSync Promise(number): {
    userCount Promise(number): fetch("https://some.com/random/api/promise")
        => (response) { response.json() }
        => (json) { json.userCount }
}

// conditionals
if userCount = 7 and (hasPermission("admin") or hasPermission("manager")) {
    // code block
} userCount > 3 & not hasPremission("user") {
    // code block
} else {
    // code block
}

if user { // if doesn't return a conditional, becomes match
    user.name = "George" { /* code block */ }
    * { /* code block */}
}
greeting string: if age {
    2 { "Hey twosies" }
    3 { "This dude is three!" }
    > 1 { "hi!" } // first match returns first, this will never run
    * { "what up" }
}

// loops
loop user in users { // array or iterable
    // code block
}
loop n in 1..10 {   // range (iterable)
    // 1,2,3,4,5,6,7,8,9,10
}
loop n number: 1 < 100 { // for

}
loop n: 1 { // do while

} n < 100