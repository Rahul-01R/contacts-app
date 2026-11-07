// =====================================
// GET HTML ELEMENTS
// =====================================

const contactsContainer =
    document.getElementById("contacts-container");

const addContactButton =
    document.getElementById("addContact");

const searchInput =
    document.getElementById("search");

const contactDialog =
    document.getElementById("contactDialog");

const contactForm =
    document.getElementById("contactForm");

const dialogTitle =
    document.getElementById("dialogTitle");

const closeDialog =
    document.getElementById("closeDialog");

const cancelContact =
    document.getElementById("cancelContact");

const contactId =
    document.getElementById("contactId");

const nameInput =
    document.getElementById("name");

const phoneInput =
    document.getElementById("phone");

const emailInput =
    document.getElementById("email");

const addressInput =
    document.getElementById("address");


// =====================================
// MESSAGE DISPLAY
// =====================================

let messageElement = document.getElementById("app-message");

if (!messageElement) {

    messageElement = document.createElement("p");

    messageElement.id = "app-message";

    contactsContainer.parentNode.insertBefore(
        messageElement,
        contactsContainer
    );

}


function showMessage(message, isError = false) {

    messageElement.textContent = message;

    messageElement.style.color =
        isError ? "red" : "green";

}


// =====================================
// GET INITIALS
// =====================================

function getInitials(name) {

    return name
        .split(" ")
        .filter(word => word.length > 0)
        .map(word => word[0])
        .join("")
        .toUpperCase();

}


// =====================================
// DISPLAY CONTACTS
// =====================================

function renderContacts(list) {

    contactsContainer.replaceChildren();

    if (list.length === 0) {

        const emptyMessage = document.createElement("p");

        emptyMessage.textContent =
            searchInput.value.trim()
                ? "No matching contacts found."
                : "No contacts yet. Add your first one.";

        contactsContainer.appendChild(emptyMessage);

        return;

    }


    list.forEach(contact => {

        const card = document.createElement("div");

        card.className = "contact-card";


        // Initials

        const initials = document.createElement("div");

        initials.className = "contact-initials";

        initials.textContent = getInitials(contact.name);


        // Name

        const name = document.createElement("h3");

        name.textContent = contact.name;


        // Phone

        const phone = document.createElement("p");

        phone.textContent = "Phone: " + contact.phone;


        // Email

        const email = document.createElement("p");

        email.textContent =
            "Email: " + (contact.email || "Not provided");


        // Address

        const address = document.createElement("p");

        address.textContent =
            "Address: " + (contact.address || "Not provided");


        // Edit Button

        const editButton = document.createElement("button");

        editButton.textContent = "Edit";

        editButton.addEventListener("click", () => {

            openEditDialog(contact);

        });


        // Delete Button

        const deleteButton = document.createElement("button");

        deleteButton.textContent = "Delete";

        deleteButton.addEventListener("click", () => {

            handleDelete(contact.id);

        });


        card.append(
            initials,
            name,
            phone,
            email,
            address,
            editButton,
            deleteButton
        );


        contactsContainer.appendChild(card);

    });

}


// =====================================
// LOAD CONTACTS FROM BACKEND
// =====================================

async function loadContacts(query = "") {

    showMessage("Loading contacts...");

    const result = await getContacts(query);


    if (!result.ok) {

        showMessage(
            result.data?.message || "Failed to load contacts.",
            true
        );

        return;

    }


    // Backend should return an array of contacts.

    const contacts = Array.isArray(result.data)
        ? result.data
        : result.data?.contacts;


    if (!Array.isArray(contacts)) {

        showMessage("Invalid contacts response from server.", true);

        return;

    }


    showMessage("");

    renderContacts(contacts);

}


// =====================================
// OPEN ADD CONTACT DIALOG
// =====================================

addContactButton.addEventListener("click", () => {

    contactForm.reset();

    contactId.value = "";

    dialogTitle.textContent = "Add Contact";

    contactDialog.showModal();

});


// =====================================
// OPEN EDIT CONTACT DIALOG
// =====================================

function openEditDialog(contact) {

    contactId.value = contact.id;

    nameInput.value = contact.name;

    phoneInput.value = contact.phone;

    emailInput.value = contact.email || "";

    addressInput.value = contact.address || "";

    dialogTitle.textContent = "Edit Contact";

    contactDialog.showModal();

}


// =====================================
// CLOSE DIALOG
// =====================================

closeDialog.addEventListener("click", () => {

    contactDialog.close();

});


cancelContact.addEventListener("click", () => {

    contactDialog.close();

});


// =====================================
// ADD / UPDATE CONTACT
// =====================================

contactForm.addEventListener("submit", async (event) => {

    event.preventDefault();


    const contactData = {

        name: nameInput.value.trim(),

        phone: phoneInput.value.trim(),

        email: emailInput.value.trim(),

        address: addressInput.value.trim()

    };


    // Frontend validation

    if (!contactData.name) {

        showMessage("Name is required.", true);

        return;

    }


    const digits = contactData.phone.replace(/\D/g, "");

    if (digits.length < 10) {

        showMessage("Enter a valid phone number.", true);

        return;

    }


    const id = contactId.value;

    let result;


    if (id) {

        // UPDATE EXISTING CONTACT

        result = await updateContact(id, contactData);

    }

    else {

        // CREATE NEW CONTACT

        result = await createContact(contactData);

    }


    // Handle errors

    if (!result.ok) {

        if (result.status === 409) {

            showMessage(
                "This phone number already exists. Use another number.",
                true
            );

        }

        else if (result.status === 400) {

            showMessage(
                result.data?.message || "Invalid contact details.",
                true
            );

        }

        else if (result.status === 404) {

            showMessage("Contact not found.", true);

        }

        else {

            showMessage(
                result.data?.message || "Unable to save contact.",
                true
            );

        }

        return;

    }


    // Successful operation

    contactDialog.close();

    contactForm.reset();

    showMessage(
        id
            ? "Contact updated successfully!"
            : "Contact created successfully!"
    );

    await loadContacts(searchInput.value);

});


// =====================================
// DELETE CONTACT
// =====================================

async function handleDelete(id) {

    const confirmed = confirm(
        "Are you sure you want to delete this contact?"
    );


    if (!confirmed) {

        return;

    }


    const result = await deleteContact(id);


    if (!result.ok) {

        showMessage(
            result.data?.message || "Unable to delete contact.",
            true
        );

        return;

    }


    showMessage("Contact deleted successfully!");

    await loadContacts(searchInput.value);

}


// =====================================
// SEARCH CONTACTS
// =====================================

let searchTimeout;


searchInput.addEventListener("input", () => {

    clearTimeout(searchTimeout);

    searchTimeout = setTimeout(() => {

        loadContacts(searchInput.value);

    }, 300);

});


// =====================================
// INITIAL LOAD
// =====================================

loadContacts();