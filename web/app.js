// Find the container where contact cards will appear
const contactsContainer = document.getElementById("contacts-container");

// Generate initials from a person's name
function getInitials(name) {
    return name
        .trim()
        .split(/\s+/)
        .map(word => word[0])
        .join("")
        .toUpperCase();
}

// Display contact cards
function renderContacts(list) {

    // Clear existing cards
    contactsContainer.replaceChildren();

    // Display a message if there are no contacts
    if (list.length === 0) {
        const message = document.createElement("p");

        message.textContent = "No contacts yet. Add your first one.";

        contactsContainer.append(message);
        return;
    }

    // Create a card for each contact
    list.forEach(function(contact) {

        const card = document.createElement("article");
        card.className = "contact-card";

        const initials = document.createElement("div");
        initials.className = "initials";
        initials.textContent = getInitials(contact.name);

        const name = document.createElement("h2");
        name.textContent = contact.name;

        const phone = document.createElement("p");
        phone.textContent = "Phone: " + contact.phone;

        const email = document.createElement("p");
        email.textContent = "Email: " + contact.email;

        const address = document.createElement("p");
        address.textContent = "Address: " + contact.address;

        // Add information to the card
        card.append(initials, name, phone, email, address);

        // Display the card
        contactsContainer.append(card);
    });
}

// Load contacts from api.js
async function loadContacts() {
    try {
        const contacts = await getContacts();
        renderContacts(contacts);
    } catch (error) {
        console.error("Unable to load contacts:", error);
    }
}

// Run when the page loads
loadContacts();
