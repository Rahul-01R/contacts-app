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

function success(data, status = 200) {
    return {
        ok: true,
        status: status,
        data: data
    };
}

function failure(status, message) {
    return {
        ok: false,
        status: status,
        data: {
            message: message
        }
    };
}

function getContacts() {
    return Promise.resolve(success([...contacts]));
}

function addContact(contact) {
    const phone = String(contact.phone || "").trim();

    if (!contact.name || !phone) {
        return Promise.resolve(
            failure(400, "Name and phone number are required.")
        );
    }

    const duplicate = contacts.some(
        item => item.phone === phone
    );

    if (duplicate) {
        return Promise.resolve(
            failure(409, "A contact with this phone number already exists.")
        );
    }

    const newContact = {
        id: Date.now(),
        name: contact.name.trim(),
        phone: phone,
        email: (contact.email || "").trim(),
        address: (contact.address || "").trim()
    };

    contacts.push(newContact);

    return Promise.resolve(success(newContact, 201));
}

function updateContact(id, updatedContact) {
    const index = contacts.findIndex(
        contact => contact.id === Number(id)
    );

    if (index === -1) {
        return Promise.resolve(
            failure(404, "Contact not found.")
        );
    }

    const phone = String(updatedContact.phone || "").trim();

    if (!updatedContact.name || !phone) {
        return Promise.resolve(
            failure(400, "Name and phone number are required.")
        );
    }

    const duplicate = contacts.some(
        contact =>
            contact.phone === phone &&
            contact.id !== Number(id)
    );

    if (duplicate) {
        return Promise.resolve(
            failure(409, "A contact with this phone number already exists.")
        );
    }

    contacts[index] = {
        ...contacts[index],
        name: updatedContact.name.trim(),
        phone: phone,
        email: (updatedContact.email || "").trim(),
        address: (updatedContact.address || "").trim()
    };

    return Promise.resolve(success(contacts[index]));
}

function deleteContact(id) {
    const index = contacts.findIndex(
        contact => contact.id === Number(id)
    );

    if (index === -1) {
        return Promise.resolve(
            failure(404, "Contact not found.")
        );
    }

    const deletedContact = contacts[index];

    contacts.splice(index, 1);

    return Promise.resolve(success(deletedContact));
}