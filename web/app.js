const contactForm = document.getElementById("contactForm");
const contactsList = document.getElementById("contactsList");
const message = document.getElementById("message");

const searchInput = document.getElementById("search");
const searchBtn = document.getElementById("searchBtn");
const clearSearchBtn = document.getElementById("clearSearchBtn");

let contacts = [];
let editingId = null;


// Display messages
function showMessage(text, isError = false) {
    message.textContent = text;
    message.style.color = isError ? "red" : "green";
}


// Extract contact array from common API response formats
function extractContacts(result) {
    if (Array.isArray(result)) {
        return result;
    }

    if (result && Array.isArray(result.data)) {
        return result.data;
    }

    if (result && result.data && Array.isArray(result.data.contacts)) {
        return result.data.contacts;
    }

    if (result && Array.isArray(result.contacts)) {
        return result.contacts;
    }

    return [];
}


// Render contacts on the page
function renderContacts(list) {
    contactsList.replaceChildren();

    if (!list || list.length === 0) {
        const emptyMessage = document.createElement("p");
        emptyMessage.textContent = "No contacts found.";
        contactsList.appendChild(emptyMessage);
        return;
    }

    list.forEach(contact => {
        const card = document.createElement("div");
        card.className = "contact-card";

        const name = document.createElement("h3");
        name.textContent = contact.name || "";

        const phone = document.createElement("p");
        phone.textContent = `Phone: ${contact.phone || ""}`;

        const email = document.createElement("p");
        email.textContent = `Email: ${contact.email || ""}`;

        const address = document.createElement("p");
        address.textContent = `Address: ${contact.address || ""}`;

        const editButton = document.createElement("button");
        editButton.textContent = "Edit";

        editButton.addEventListener("click", () => {
            editContact(contact);
        });

        const deleteButton = document.createElement("button");
        deleteButton.textContent = "Delete";

        deleteButton.addEventListener("click", () => {
            deleteContact(contact.id);
        });

        card.append(name, phone, email, address, editButton, deleteButton);

        contactsList.appendChild(card);
    });
}


// Load all contacts
async function loadContacts() {
    try {
        showMessage("Loading contacts...");

        const result = await api.listContacts();

        contacts = extractContacts(result);

        renderContacts(contacts);

        showMessage("Contacts loaded successfully.");

    } catch (error) {
        console.error("Loading error:", error);

        showMessage(
            "Unable to load contacts. Check whether the backend is running.",
            true
        );
    }
}


// Add or update a contact
contactForm.addEventListener("submit", async function (event) {
    event.preventDefault();

    const contactData = {
        name: document.getElementById("name").value.trim(),
        phone: document.getElementById("phone").value.trim(),
        email: document.getElementById("email").value.trim(),
        address: document.getElementById("address").value.trim()
    };

    if (!contactData.name || !contactData.phone) {
        showMessage("Name and phone are required.", true);
        return;
    }

    const phoneDigits = contactData.phone.replace(/\D/g, "");

    if (phoneDigits.length < 10) {
        showMessage("Enter a valid phone number.", true);
        return;
    }

    try {
        if (editingId === null) {

            // Correct: call through the api object
            await api.createContact(contactData);

            showMessage("Contact added successfully.");

        } else {

            // Correct: call through the api object
            await api.updateContact(editingId, contactData);

            showMessage("Contact updated successfully.");

            editingId = null;
        }

        contactForm.reset();

        document.getElementById("addContact").textContent = "Add Contact";

        await loadContacts();

    } catch (error) {
        console.error("Save error:", error);

        showMessage(error.message || "Unable to save contact.", true);
    }
});


// Edit contact
function editContact(contact) {
    editingId = contact.id;

    document.getElementById("name").value = contact.name || "";
    document.getElementById("phone").value = contact.phone || "";
    document.getElementById("email").value = contact.email || "";
    document.getElementById("address").value = contact.address || "";

    document.getElementById("addContact").textContent = "Update Contact";

    showMessage("Editing contact.");
}


// Delete contact
async function deleteContact(id) {
    if (id === undefined || id === null) {
        showMessage("Contact ID is missing.", true);
        return;
    }

    const confirmed = confirm("Are you sure you want to delete this contact?");

    if (!confirmed) {
        return;
    }

    try {
        // Correct: call through the api object
        await api.deleteContact(id);

        showMessage("Contact deleted successfully.");

        await loadContacts();

    } catch (error) {
        console.error("Delete error:", error);

        showMessage(error.message || "Unable to delete contact.", true);
    }
}


// Search contacts
async function searchContacts() {
    const query = searchInput.value.trim();

    if (!query) {
        await loadContacts();
        return;
    }

    try {
        const result = await api.searchExact(query);

        const searchResults = extractContacts(result);

        renderContacts(searchResults);

        showMessage("Search completed.");

    } catch (error) {
        console.error("Search error:", error);

        showMessage(error.message || "Search failed.", true);
    }
}


// Search button
searchBtn.addEventListener("click", searchContacts);


// Search when Enter is pressed
searchInput.addEventListener("keydown", function (event) {
    if (event.key === "Enter") {
        searchContacts();
    }
});


// Clear search
clearSearchBtn.addEventListener("click", function () {
    searchInput.value = "";
    loadContacts();
});


// Initial loading
document.addEventListener("DOMContentLoaded", function () {
    loadContacts();
});