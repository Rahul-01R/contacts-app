// Mock contact data

let contacts = [
    {
        id: 1,
        name: "Rahul Sharma",
        phone: "9876543210",
        email: "rahul@example.com",
        address: "Visakhapatnam"
    },
    {
        id: 2,
        name: "Priya Kumar",
        phone: "9123456780",
        email: "priya@example.com",
        address: "Hyderabad"
    },
    {
        id: 3,
        name: "Arjun Reddy",
        phone: "9988776655",
        email: "arjun@example.com",
        address: "Vijayawada"
    },
    {
        id: 4,
        name: "Sneha Rao",
        phone: "9012345678",
        email: "sneha@example.com",
        address: "Chennai"
    },
    {
        id: 5,
        name: "Kiran Kumar",
        phone: "9090909090",
        email: "",
        address: "Bengaluru"
    }
];


// Generate the next contact ID

let nextId = Math.max(0, ...contacts.map(contact => contact.id)) + 1;


// READ: Get all contacts

async function getContacts() {
    return contacts.map(contact => ({ ...contact }));
}


// CREATE: Add a contact

async function createContact(contactData) {
    const newContact = {
        id: nextId++,
        name: contactData.name,
        phone: contactData.phone,
        email: contactData.email,
        address: contactData.address
    };

    contacts.push(newContact);

    return { ...newContact };
}


// UPDATE: Edit a contact

async function updateContact(id, updatedData) {
    const index = contacts.findIndex(
        contact => contact.id === Number(id)
    );

    if (index === -1) {
        throw new Error("Contact not found.");
    }

    contacts[index] = {
        ...contacts[index],
        name: updatedData.name,
        phone: updatedData.phone,
        email: updatedData.email,
        address: updatedData.address
    };

    return { ...contacts[index] };
}


// DELETE: Remove a contact

async function deleteContact(id) {
    const index = contacts.findIndex(
        contact => contact.id === Number(id)
    );

    if (index === -1) {
        throw new Error("Contact not found.");
    }

    contacts.splice(index, 1);

    return true;
}