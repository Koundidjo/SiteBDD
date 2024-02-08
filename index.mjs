import express from "express";
import { PrismaClient } from "@prisma/client";
const app = express();

const prisma = new PrismaClient();

app.use(express.json());
app.use(express.urlencoded());

// Pages statiques
app.use(express.static("public"));

// Retourne tous les dispositifs de la base
app.get("/devices", async (req, res) => {
  const allDevices = await prisma.device.findMany();
  res.status(200).json(allDevices);
});

// Route pour afficher toutes les données de la table Data
app.get('/data', async (req, res) => {
  try {
    const allData = await prisma.data.findMany();

    res.status(200).json(allData);
  } catch (error) {
    console.error('Erreur lors de la récupération de toutes les données de la table Data:', error);
    res.status(500).json({ error: 'Erreur interne du serveur' });
  }
});
app.get("/device/:id", async (req, res) => {
  const { id } = req.params;
  const device = await prisma.device.findUnique({
    where: { id: Number(id) },
  });
  res.json(device);
});

// Crée un nouveau dispositif avec les données du formulaire
app.post("/new_device", async (req, res) => {
  const { nom, localisation } = req.body;
  const result = await prisma.device.create({
    data: {
      nom: nom,
      localisation: localisation,
    },
  });
  res.json(result);
});

// Endpoint pour mettre à jour un dispositif par son ID
app.put("/update_device/:id", async (req, res) => {
  const { id } = req.params;
  const { nom, localisation } = req.body;

  const updatedDevice = await prisma.device.update({
    where: { id: Number(id) },
    data: {
      nom: nom,
      localisation: localisation,
    },
  });

  res.json(updatedDevice);
});

// Endpoint pour supprimer un dispositif par son ID
app.delete("/delete_device/:id", async (req, res) => {
  const { id } = req.params;

  const deletedDevice = await prisma.device.delete({
    where: { id: Number(id) },
  });

  res.json(deletedDevice);
});

// Route pour lire les valeurs de PM2.5 pour un dispositif spécifique dans une plage horaire
app.get('/data/:id_device/:debut/:fin', async (req, res) => {
  try {
    const { id_device, debut, fin } = req.params;

    // Ajoutez ici la logique pour récupérer les valeurs de PM2.5 pour le dispositif spécifié
    // dans la plage horaire spécifiée
    const pm25ValuesForDevice = await prisma.data.findMany({
      where: {
        deviceId: parseInt(id_device, 10),
        timestamp: {
          gte: new Date(debut),
          lte: new Date(fin),
        },
      },
      select: {
        concentration: true,
        timestamp: true,
        deviceId: true,
      },
    });

    res.status(200).json(pm25ValuesForDevice);
  } catch (error) {
    console.error('Erreur lors de la récupération des valeurs de PM2.5 pour le dispositif dans la plage horaire:', error);
    res.status(500).json({ error: 'Erreur interne du serveur' });
  }
});
// Lancement du serveur
app.listen(8080, () => {
  console.log("Serveur à l'écoute sur le port 8080");
});
