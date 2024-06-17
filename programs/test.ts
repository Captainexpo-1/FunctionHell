let foo: Function = (): Function => {
    return (a: number, b: number): number => {
        return a / b;
    }
}

let x: Function = foo()
x(1, 2, ) == 2
