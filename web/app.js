// Get HTML elements

const contactsList = document.getElementById("contactsList");

const searchInput = document.getElementById("search");

const addButton = document.getElementById("addContact");

const formSection = document.getElementById("formSection");

const contactForm = document.getElementById("contactForm");

const formTitle = document.getElementById("formTitle");

const cancelButton = document.getElementById("cancelButton");

const message = document.getElementById("message");


// Store contacts

let contacts = [];


// Display messages

function showMessage(text, isError = false) {

    message.textContent = text;

    message.style.color = isError ? "red" : "green";

}


// Get contacts from backend response

function extractContacts(data) {

    if (Array.isArray(data)) {
        return data;
    }

    if (data && Array.isArray(data.contacts)) {
        return data.contacts;
    }

    if (data && Array.isArray(data.data)) {
        return data.data;
    }

    return [];

}


// Load all contacts

async function loadContacts() {

    contactsList.textContent = "Loading contacts...";

    const result = await api.listContacts();

    if (!result.ok) {

        contactsList.textContent = "Unable to load contacts.";

        showMessage(
            result.data?.message || "Server connection failed.",
            true
        );

        return;
    }

    contacts = extractContacts(result.data);

    displayContacts(contacts);

}


// Display contacts

function displayContacts(list) {

    contactsList.replaceChildren();

    if (list.length === 0) {

        const emptyMessage = document.createElement("p");

        emptyMessage.textContent =
            "No contacts yet. Add your first one.";

        contactsList.appendChild(emptyMessage);

        return;
    }

    list.forEach(contact => {

        const card = document.createElement("div");

        card.className = "contact-card";


        const name = document.createElement("h3");

        name.textContent = contact.name || "No name";


        const phone = document.createElement("p");

        phone.textContent = "Phone: " + (contact.phone || "");


        const email = document.createElement("p");

        email.textContent = "Email: " + (contact.email || "");


        const address = document.createElement("p");

        address.textContent = "Address: " + (contact.address || "");


        const actions = document.createElement("div");

        actions.className = "contact-actions";


        const editButton = document.createElement("button");

        editButton.textContent = "Edit";

        editButton.className = "edit-button";

        editButton.addEventListener("click", () => {

            editContact(contact);

        });


        const deleteButton = document.createElement("button");

        deleteButton.textContent = "Delete";

        deleteButton.className = "delete-button";

        deleteButton.addEventListener("click", () => {

            deleteContact(contact.id);

        });


        actions.append(editButton, deleteButton);

        card.append(name, phone, email, address, actions);

        contactsList.appendChild(card);

    });

}


// Open add contact form

addButton.addEventListener("click", () => {

    contactForm.reset();

    document.getElementById("contactId").value = "";

    formTitle.textContent = "Add New Contact";

    formSection.classList.remove("hidden");

    showMessage("");

});


// Close form

cancelButton.addEventListener("click", () => {

    formSection.classList.add("hidden");

    contactForm.reset();

});


// Validate contact details

function validateContact(contact) {

    if (!contact.name.trim()) {

        showMessage("Name is required.", true);

        return false;

    }

    const phoneDigits = contact.phone.replace(/\D/g, "");

    if (phoneDigits.length < 10) {

        showMessage("Phone number must contain at least 10 digits.", true);

        return false;

    }

    if (contact.email && !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(contact.email)) {

        showMessage("Enter a valid email address.", true);

        return false;

    }

    return true;

}


// Add or update contact

contactForm.addEventListener("submit", async (event) => {

    event.preventDefault();


    const id = document.getElementById("contactId").value;


    const contact = {

        name: document.getElementById("name").value.trim(),

        phone: document.getElementById("phone").value.trim(),

        email: document.getElementById("email").value.trim(),

        address: document.getElementById("address").value.trim()

    };


    if (!validateContact(contact)) {
        return;
    }


    let result;


    if (id) {

        // Update existing contact

        result = await api.updateContact(id, contact);

    } else {

        // Add new contact

        result = await api.createContact(contact);

    }


    if (!result.ok) {

        showMessage(
            result.data?.message || "Operation failed.",
            true
        );

        return;

    }


    showMessage(
        id ? "Contact updated successfully!" : "Contact added successfully!"
    );

    formSection.classList.add("hidden");

    contactForm.reset();

    await loadContacts();

});


// Edit contact

function editContact(contact) {

    document.getElementById("contactId").value = contact.id;

    document.getElementById("name").value = contact.name || "";

    document.getElementById("phone").value = contact.phone || "";

    document.getElementById("email").value = contact.email || "";

    document.getElementById("address").value = contact.address || "";


    formTitle.textContent = "Edit Contact";

    formSection.classList.remove("hidden");

    showMessage("");

}


// Delete contact

async function deleteContact(id) {

    const confirmDelete = confirm(
        "Are you sure you want to delete this contact?"
    );

    if (!confirmDelete) {
        return;
    }


    const result = await api.deleteContact(id);


    if (!result.ok) {

        showMessage(
            result.data?.message || "Unable to delete contact.",
            true
        );

        return;

    }


    showMessage("Contact deleted successfully!");

    await loadContacts();

}


// Search contacts

let searchTimer;

searchInput.addEventListener("input", () => {

    clearTimeout(searchTimer);

    searchTimer = setTimeout(async () => {

        const query = searchInput.value.trim();

        if (query === "") {

            displayContacts(contacts);

            return;

        }


        const result = await api.searchExact(query);


        if (!result.ok) {

            showMessage(
                result.data?.message || "Search failed.",
                true
            );

            return;

        }


        const searchResults = extractContacts(result.data);

        displayContacts(searchResults);

    }, 300);

});


// Load contacts when page opens

loadContacts();