function Wizard(name, house, pet) {
    this.name = name;
    this.house = house;
    this.pet = pet;
    
    this.greet = () =>{
        return `I am ${this.name} from ${this.house}`;
    }
}

Wizard.prototype.petName;
Wizard.prototype.info = function() {
    return `i have a ${this.pet} named ${this.petName}`
};
let harry = new Wizard("Harry potter", "gryggindor", "owl");
harry.petName = "Hedwig";
console.log(harry.info());