const budget = () => {
    let balance = 0;
    let changeBal = (val) => {
        return balance += val;
    }
    const deposit20 = () => changeBal(20);
    const withdrow20 = () => changeBal(-20);
    const check = () => balance;
    
    return {
        deposit20: deposit20,
        check: check,
        withdrow20: withdrow20
    }
}

let wallet = budget();
console.log(wallet);
wallet.deposit20();
wallet.withdrow20();
wallet.deposit20();
wallet.deposit20();
console.log(wallet.check());